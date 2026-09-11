from __future__ import annotations
from pathlib import Path
from typing import Optional

from src.splat.util import options

from src.splat.util import log
from src.splat.segtypes.common.bin import CommonSegBin
from src.splat.segtypes.segment import Segment, SegmentType
from src.splat.segtypes.common.segment import CommonSegment


class N64SegUvft(CommonSegBin):
    def out_path(self) -> Optional[Path]:
        return options.opts.asset_path / self.dir / f"{self.name}.uvft"

    @property
    def statistics_type(self) -> SegmentType:
        stats_type = self.type
        if self.is_name_default():
            stats_type = "unk"
        return stats_type

    def get_linker_entries(self):
        from src.splat.segtypes.linker_entry import LinkerEntry

        return [
            LinkerEntry(
                self,
                [options.opts.asset_path / self.dir / f"{self.name}.bin"],
                options.opts.asset_path
                / self.dir
                / f"{self.name}",
                self.get_linker_section_order(),
                self.get_linker_section_linksection(),
                self.is_noload(),
            )
        ]
