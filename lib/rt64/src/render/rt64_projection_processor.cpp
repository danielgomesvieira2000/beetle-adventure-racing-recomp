//
// RT64
//

#include <cstdio>
#include <cstdlib>
#include "rt64_projection_processor.h"

#include "common/rt64_math.h"
#include "hle/rt64_bar_hud.h"
#include "hle/rt64_workload_queue.h"

namespace RT64 {
    inline void adjustProjectionMatrix(interop::float4x4 &matrix, const float aspectRatioScale) {
        matrix[0][0] *= aspectRatioScale;
        matrix[1][0] *= aspectRatioScale;
        matrix[2][0] *= aspectRatioScale;
        matrix[3][0] *= aspectRatioScale;
    }
    
namespace {
    // RT64's projection type as the classifier's own kind, so the identity says what the layer is.
    inline RT64::BarHud::ProjKind barProjKind(RT64::Projection::Type type) {
        using RT64::Projection;
        using RT64::BarHud::ProjKind;
        switch (type) {
        case Projection::Type::Perspective:  return ProjKind::Perspective;
        case Projection::Type::Orthographic: return ProjKind::Orthographic;
        case Projection::Type::Rectangle:    return ProjKind::Rectangle;
        default:                             return ProjKind::Other;
        }
    }
}

    // ProjectionProcessor

    ProjectionProcessor::ProjectionProcessor() { }

    ProjectionProcessor::~ProjectionProcessor() {
        bufferUploader.reset(nullptr);
    }

    void ProjectionProcessor::setup(RenderWorker *worker) {
        bufferUploader = std::make_unique<BufferUploader>(worker->device);
    }

    void ProjectionProcessor::process(const ProcessParams &p) {
        for (uint32_t w : p.curFrame->workloads) {
            Workload &workload = p.workloadQueue->workloads[w];
            DrawData &drawData = workload.drawData;

            // Copy the data.
            drawData.modViewTransforms = drawData.viewTransforms;
            drawData.modProjTransforms = drawData.projTransforms;
            drawData.modViewProjTransforms = drawData.viewProjTransforms;
            drawData.prevViewTransforms = drawData.viewTransforms;
            drawData.prevProjTransforms = drawData.projTransforms;
            drawData.prevViewProjTransforms = drawData.viewProjTransforms;
        }

        for (size_t s = 0; s < p.curFrame->perspectiveScenes.size(); s++) {
            processScene(p, p.curFrame->perspectiveScenes[s], s);
        }

        for (size_t s = 0; s < p.curFrame->orthographicScenes.size(); s++) {
            processScene(p, p.curFrame->orthographicScenes[s], s);
        }
    }

    void ProjectionProcessor::processScene(const ProcessParams &p, const GameScene &scene, size_t sceneIndex) {
        for (size_t i = 0; i < scene.projections.size(); i++) {
            const GameIndices::Projection &sceneProj = scene.projections[i];
            Workload &workload = p.workloadQueue->workloads[sceneProj.workloadIndex];
            DrawData &drawData = workload.drawData;
            const FramebufferPair &fbPair = workload.fbPairs[sceneProj.fbPairIndex];
            const Projection &proj = fbPair.projections[sceneProj.projectionIndex];

            // BAR: some of the game's 2D is geometry under an orthographic projection rather than
            // texture rectangles, and those layers are decided here rather than in the RDP. They do
            // not all want the same thing -- the speedometer's needle has to follow its dial to the
            // left edge, while the pause screen's backdrop has to cover the whole widened frame --
            // so the answer is per projection, and taggable by the number RT64's own debugger shows.
            // See BarHud::classifyOrtho.
            //
            // Only a layer the game left unpinned is ours to decide; one that already carries an
            // origin is left exactly as it was.
            uint16_t viewportOrigin = drawData.viewportOrigins[proj.transformsIndex];
            BarHud::Class orthoClass = BarHud::Class::Center;
            if (viewportOrigin == G_EX_ORIGIN_NONE) {
                orthoClass = BarHud::classifyProjection(barProjKind(proj.type),
                    sceneProj.projectionIndex, proj.scissorRect, true);
                viewportOrigin = BarHud::viewportOriginFor(orthoClass);
            }
            assert(proj.transformsIndex > 0);

            // Skip projections that didn't actually draw anything.
            if (proj.scissorRect.isNull()) {
                continue;
            }

            // Check the current mapping for the projection.
            const interop::float4x4 *prevProjMatrix = nullptr;
            const interop::float4x4 *prevViewMatrix = nullptr;
            const RigidBody *rigidBody = nullptr;
            const GameFrameMap::WorkloadMap &workloadMap = p.curFrame->frameMap.workloads[sceneProj.workloadIndex];
            if ((p.prevFrame != nullptr) && workloadMap.mapped && !workload.debuggerCamera.enabled) {
                const GameFrameMap::ViewProjectionMap &viewProjMap = workloadMap.viewProjections[proj.transformsIndex];
                if (viewProjMap.mapped) {
                    const Workload &prevWorkload = p.workloadQueue->workloads[workloadMap.prevWorkloadIndex];
                    prevViewMatrix = &prevWorkload.drawData.viewTransforms[viewProjMap.prevTransformIndex];
                    prevProjMatrix = &prevWorkload.drawData.projTransforms[viewProjMap.prevTransformIndex];
                    rigidBody = &viewProjMap.rigidBody;
                }
            }

            const uint32_t curProjGroupIndex = workload.drawData.viewProjTransformGroups[proj.transformsIndex];
            const TransformGroup &curProjGroup = workload.drawData.transformGroups[curProjGroupIndex];
            bool adjustAspectRatio = (curProjGroup.aspectMode == G_EX_ASPECT_ADJUST);
            if (curProjGroup.aspectMode == G_EX_ASPECT_AUTO) {
                FixedRect intersectionRect = proj.scissorRect;
                if (proj.usesViewport()) {
                    const interop::RSPViewport &viewport = drawData.rspViewports[proj.transformsIndex];
                    const int16_t *viewportClipRatios = &drawData.viewportClipRatios[proj.transformsIndex * 4];
                    intersectionRect = intersectionRect.intersection(viewport.rect(viewportClipRatios));
                }

                if (!intersectionRect.isEmpty()) {
                    bool coversWholeWidth = (intersectionRect.ulx <= fbPair.scissorRect.ulx) && (intersectionRect.lrx >= fbPair.scissorRect.lrx);

                    // BAR: a game that insets its 3D viewport never satisfies the strict test above,
                    // and so never gets widescreen.
                    //
                    // BAR draws racing into a 275x207 rectangle inset at (22,17) -- a CRT
                    // overscan-safe area -- while the framebuffer's own scissor stays 0..320. The
                    // strict test asks the projection to reach the framebuffer's edges, so 22 <= 0
                    // is false, the projection is left unwidened, and Expand appears to do nothing
                    // during a race while still working on menus, whose 2D content does use the
                    // full width. Accepting a projection that covers most of the width fixes that.
                    //
                    // BAR_ASPECT_COVER is the percentage of the framebuffer width a projection must
                    // cover to qualify. BAR's racing viewport covers 275/320 = 86%, so the default is
                    // 80; set it to 100 to restore the strict upstream test.
                    if (!coversWholeWidth) {
                        static const int coverPercent = [] {
                            const char *e = std::getenv("BAR_ASPECT_COVER");
                            int parsed = 0;
                            if ((e != nullptr) && (sscanf(e, "%d", &parsed) == 1) && (parsed > 0) && (parsed <= 100)) {
                                return parsed;
                            }
                            return 80;
                        }();
                        if (coverPercent < 100) {
                            const int32_t fbWidth = fbPair.scissorRect.width(true, true);
                            const int32_t interWidth = intersectionRect.width(true, true);
                            coversWholeWidth = (fbWidth > 0) && ((int64_t(interWidth) * 100) >= (int64_t(fbWidth) * coverPercent));
                        }
                    }

                    bool horizontalRatio = (intersectionRect.width(true, true) > intersectionRect.height(true, true));
                    adjustAspectRatio = (viewportOrigin == G_EX_ORIGIN_NONE) && coversWholeWidth && horizontalRatio;

                    // BAR: Stretch means "do not compensate this projection's aspect ratio", so the
                    // layer's own 320-wide space is mapped across the full widened frame and its
                    // content stretches with it. That is the opposite of what the compensation
                    // above does, which keeps the content proportional and simply reveals more
                    // empty space either side of it -- which is what a full-screen backdrop looked
                    // like before this existed: correctly centred, and still 4:3 wide.
                    if ((orthoClass == BarHud::Class::Stretch) || (orthoClass == BarHud::Class::Cover)) {
                        adjustAspectRatio = false;
                    }

                    // BAR_DBG_PROJ=1 shows, per projection, whether it is being widened and why not.
                    static const bool projDbg = std::getenv("BAR_DBG_PROJ") != nullptr;
                    if (projDbg) {
                        static int32_t lastI = -1, lastF = -1, lastT = -1; static bool lastAdj = false;
                        const int32_t iw = intersectionRect.width(true, true);
                        const int32_t fw = fbPair.scissorRect.width(true, true);
                        const int32_t ty = int32_t(proj.type);
                        if ((iw != lastI) || (fw != lastF) || (adjustAspectRatio != lastAdj) || (ty != lastT)) {
                            lastI = iw; lastF = fw; lastAdj = adjustAspectRatio; lastT = ty;
                            fprintf(stderr, "[proj] type=%d projWidth=%d fbScissorWidth=%d covers=%d horiz=%d origin=%d -> widen=%d\n",
                                int(ty), int(iw), int(fw), coversWholeWidth ? 1 : 0, horizontalRatio ? 1 : 0,
                                int(viewportOrigin), adjustAspectRatio ? 1 : 0);
                            fflush(stderr);
                        }
                    }
                }
            }
 
            float projRatioScale = adjustAspectRatio ? (1.0f / p.aspectRatioScale) : 1.0f;
            interop::float4x4 &viewMatrix = drawData.modViewTransforms[proj.transformsIndex];
            interop::float4x4 &projMatrix = drawData.modProjTransforms[proj.transformsIndex];
            interop::float4x4 &viewProjMatrix = drawData.modViewProjTransforms[proj.transformsIndex];
            viewMatrix = drawData.viewTransforms[proj.transformsIndex];
            projMatrix = drawData.projTransforms[proj.transformsIndex];
            viewProjMatrix = drawData.viewProjTransforms[proj.transformsIndex];

            // Debugger camera.
            if (workload.debuggerCamera.enabled && (proj.type == Projection::Type::Perspective) && (workload.debuggerCamera.sceneIndex == sceneIndex)) {
                viewMatrix = workload.debuggerCamera.viewMatrix;
                projMatrix = workload.debuggerCamera.projMatrix;
            }

            adjustProjectionMatrix(projMatrix, projRatioScale);

            // BAR: Cover magnifies the layer's content about its centre, on top of the stretch.
            //
            // Stretching a layer maps its own 320x240 space across the frame; it does nothing about
            // geometry drawn inset WITHIN that space, and BAR authors its full-screen 2D into a
            // rectangle inset for a television. So the pause screen's backdrop, with its projection
            // stretched, still stopped about 21 of 240 short at the top -- the inset, exactly. No
            // scaling of the layer can close that, because the quad is a fixed size inside it.
            //
            // Magnifying the projection is what closes it: scale x and y by the ratio that puts the
            // inset rectangle on the frame's edges, and the quad reaches them. Anything the layer
            // draws outside that rectangle is pushed off the frame, which is why this is a class to
            // be tagged onto a backdrop rather than anything applied by default.
            if (orthoClass == BarHud::Class::Cover) {
                float coverX = 1.0f;
                float coverY = 1.0f;
                BarHud::coverScale(orthoClass, &coverX, &coverY);
                projMatrix[0][0] *= coverX;
                projMatrix[1][0] *= coverX;
                projMatrix[2][0] *= coverX;
                projMatrix[3][0] *= coverX;
                projMatrix[0][1] *= coverY;
                projMatrix[1][1] *= coverY;
                projMatrix[2][1] *= coverY;
                projMatrix[3][1] *= coverY;
            }

            interop::float4x4 &prevViewTransform = drawData.prevViewTransforms[proj.transformsIndex];
            interop::float4x4 &prevProjTransform = drawData.prevProjTransforms[proj.transformsIndex];
            if ((prevProjMatrix != nullptr) && (prevViewMatrix != nullptr) && (rigidBody != nullptr)) {
                const interop::float4x4 curViewTransform = viewMatrix;
                const interop::float4x4 curProjTransform = projMatrix;
                interop::float4x4 adjustedPrevProj = *prevProjMatrix;
                adjustProjectionMatrix(adjustedPrevProj, projRatioScale);
                viewMatrix = rigidBody->lerp(p.curFrameWeight, *prevViewMatrix, curViewTransform, true);
                prevViewTransform = rigidBody->lerp(p.prevFrameWeight, *prevViewMatrix, curViewTransform, true);

                // We only interpolate the projection if the view matrix has been interpolated.
                const bool interpolateProjection = rigidBody->lerpTranslation || rigidBody->lerpRotation;
                if (interpolateProjection) {
                    projMatrix = lerpMatrix(adjustedPrevProj, curProjTransform, p.curFrameWeight);
                    prevProjTransform = lerpMatrix(adjustedPrevProj, curProjTransform, p.prevFrameWeight);
                }
                else {
                    projMatrix = curProjTransform;
                    prevProjTransform = curProjTransform;
                }
            }
            else {
                prevViewTransform = viewMatrix;
                prevProjTransform = projMatrix;
            }

            viewProjMatrix = hlslpp::mul(viewMatrix, projMatrix);

            interop::float4x4 &prevViewProjTransform = drawData.prevViewProjTransforms[proj.transformsIndex];
            prevViewProjTransform = hlslpp::mul(prevViewTransform, prevProjTransform);
        }
    }

    void ProjectionProcessor::upload(const ProcessParams &p) {
        uploads.clear();

        for (uint32_t w : p.curFrame->workloads) {
            Workload &workload = p.workloadQueue->workloads[w];
            const DrawData &drawData = workload.drawData;
            DrawBuffers &drawBuffers = workload.drawBuffers;
            std::pair<size_t, size_t> uploadRange = { 0, drawData.viewProjTransforms.size() };
            uploads.emplace_back(BufferUploader::Upload{ drawData.modViewProjTransforms.data(), uploadRange, sizeof(interop::float4x4), RenderBufferFlag::STORAGE, { }, &drawBuffers.viewProjTransformsBuffer });
        }

        bufferUploader->submit(p.worker, uploads);
    }
};