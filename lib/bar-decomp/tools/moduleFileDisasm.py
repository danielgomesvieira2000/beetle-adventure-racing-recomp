from __future__ import annotations

import argparse
from pathlib import Path
import struct
from spimdisasm import common
from spimdisasm import elf32
from spimdisasm import mips
from spimdisasm import frontendCommon as fec
from spimdisasm import __version__
from spimdisasm.common.Relocation import RelocationInfo, RelocType


MODULE_FILES_CODE_BYTES_START: int = 0x50
MODULE_FILES_TEXT_SEC_FAKE_VADDR: int = 0x400000

MIPS_RELOC_HI16 = 0
MIPS_RELOC_LO16 = 1
MIPS_RELOC_26 = 2
MIPS_UNK_RELOC_3 = 3
MIPS_UNK_RELOC_4 = 4
MIPS_RELOC_32 = 5
MIPS_UNK_RELOC_6 = 6

INSTRUCTION_SECTION_TEXT = 0
INSTRUCTION_SECTION_DATA = 1
INSTRUCTION_SECTION_RODATA = 2

SYM_SECTION_TEXT = 0
SYM_SECTION_RODATA = 1
SYM_SECTION_DATA = 2
SYM_SECTION_BSS = 3

class ModuleFileParse:
    class ModuleFileHeaderDataClass:
        formTag: int # 0x0
        fileSize: int # 0x4
        moduTag: int # 0x8
        padTag: int  # 0xC
        padSize: int # 0x10
        padContents: int # 0x14
        commTag: int # 0x18
        commSize: int # 0x1C

        # Module comm info
        headerSize: int # 0x20
        entryPointOffset: int # 0x24
        textSize: int # 0x28
        rodataSize: int # 0x2C
        dataSize: int # 0x30
        bssSize: int # 0x34
        relocCount: int # 0x38
        nameTag: int # 0x3C
        relaContents: int # 0x40


        def __init__(self, array_of_bytes: bytearray) -> None:
            self.formTag = struct.unpack_from(">4s", array_of_bytes, 0)[0]
            self.moduTag = struct.unpack_from(">4s", array_of_bytes, 8)[0]
            self.padTag = struct.unpack_from(">4s", array_of_bytes, 0xC)[0]
            self.padSize = struct.unpack_from(">I", array_of_bytes, 0x10)[0]
            self.padContents = struct.unpack_from(">4s", array_of_bytes, 0x14)[0]
            self.commTag = struct.unpack_from(">4s", array_of_bytes, 0x18)[0]
            self.commSize = struct.unpack_from(">I", array_of_bytes, 0x1C)[0]
            self.headerSize = struct.unpack_from(">I", array_of_bytes, 0x20)[0]
            self.entryPointOffset = struct.unpack_from(">I", array_of_bytes, 0x24)[0]
            self.textSize = struct.unpack_from(">I", array_of_bytes, 0x28)[0]
            self.rodataSize = struct.unpack_from(">I", array_of_bytes, 0x2C)[0]
            self.dataSize = struct.unpack_from(">I", array_of_bytes, 0x30)[0]
            self.bssSize = struct.unpack_from(">I", array_of_bytes, 0x34)[0]
            self.relocCount = struct.unpack_from(">I", array_of_bytes, 0x38)[0]
            self.nameTag = struct.unpack_from(">4s", array_of_bytes, 0x3C)[0]
            self.relaContents = struct.unpack_from(">I", array_of_bytes, 0x40)[0]

    class RelaReloc:
        def __init__(self, romOffset, symSection, targetInstructionSection, relocType) -> None:
            self.romOffset = romOffset
            self.symSection = symSection
            self.targetInstructionSection = targetInstructionSection
            self.relocType = relocType

    __moduleFileHeader: ModuleFileHeaderDataClass
    text_rom_start: int
    text_rom_end: int
    data_rom_start: int
    data_rom_end: int
    rodata_rom_start: int
    rodata_rom_end: int
    bss_rom_start: int
    bss_rom_end: int
    entry_offset: int
    code_end_offset: int
    textFakeVAddr: int
    rodataFakeVAddr: int
    dataFakeVAddr: int
    bssFakeVAddr: int
    debug = True

    def getEntryFuncOffset(self):
        return self.__moduleFileHeader.entryPointOffset

    def getBssSize(self):
        return self.__moduleFileHeader.bssSize

    def getTextSize(self):
        return self.__moduleFileHeader.textSize
    
    def getRodataSize(self):
        return self.__moduleFileHeader.rodataSize
    
    def getDataSize(self):
        return self.__moduleFileHeader.dataSize
    
    def relocTypeToString(self, reloc_type):
        return {
            MIPS_RELOC_HI16: "R_MIPS_HI16",
            MIPS_RELOC_LO16: "R_MIPS_LO16",
            MIPS_RELOC_32: "R_MIPS_32",
            MIPS_UNK_RELOC_3: "MIPS_UNK_RELOC_3?",
            MIPS_UNK_RELOC_4: "MIPS_UNK_RELOC_4",
            MIPS_RELOC_26: "R_MIPS_26",
            MIPS_UNK_RELOC_6: "MIPS_UNK_RELOC_6",
        }.get(reloc_type, "UNKNOWN")
    
    def relaRelocTypeToProperRelocType(self, reloc_type):
        return {
            MIPS_RELOC_HI16: RelocType.MIPS_HI16,
            MIPS_RELOC_LO16: RelocType.MIPS_LO16,
            MIPS_RELOC_32: RelocType.MIPS_32,
            MIPS_RELOC_26: RelocType.MIPS_26
        }.get(reloc_type, -1)
    
    def relocInstructionSectionToString(self, section):
        return {
            INSTRUCTION_SECTION_TEXT: ".text",
            INSTRUCTION_SECTION_DATA: ".data",
            INSTRUCTION_SECTION_RODATA: ".rodata",
        }.get(section, "UNKNOWN")
    
    def symbolSectionToString(self, sym_section):
        return {
            SYM_SECTION_TEXT: ".text",
            SYM_SECTION_DATA: ".data",
            SYM_SECTION_RODATA: ".rodata",
            SYM_SECTION_BSS: ".bss",
        }.get(sym_section, "UNKNOWN")
    
    def scanRela(self):
        mdbgSize = 0x28
        relaSize = 0x8
        relaOffset = self.text_rom_start + self.getTextSize() + self.getRodataSize() + self.getDataSize() + mdbgSize
        relaStartOffset = relaOffset + 0x8
        relaTag = struct.unpack_from(">4s", self.bytes, relaOffset)[0]
        relaSize = struct.unpack_from(">I", self.bytes, relaOffset + 0x4)[0]
        print(relaTag)        
        print(hex(relaSize))

        self.relocs = []

        for i in range(0, relaSize - 0x4, 4):
            data = struct.unpack_from(">I", self.bytes, relaStartOffset + i)[0]
            symbolSection = data >> 0x1C
            targetInstructionSection = (data & 0x0C000000) >> 0x1A
            relocType = (data & 0x03C00000) >> 0x16
            addend  = (data & 0x003FFFFF) << 1

            match targetInstructionSection:
                case 0:
                    base = MODULE_FILES_CODE_BYTES_START
                case 1:
                    base = MODULE_FILES_CODE_BYTES_START + self.getTextSize()
                case 2:
                    base = MODULE_FILES_CODE_BYTES_START + self.getTextSize() + self.getRodataSize()
                case _:
                    base = -1


            #print(f"Sym section: {self.symbolSectionToString(symbolSection)}")
            #print(f"Target instruction section: {self.relocInstructionSectionToString(targetInstructionSection)}")
            #print(f"Relocation type: {self.relocTypeToString(relocType)}")
            #print(f"Offset: {hex(base + addend)}")
            self.relocs.append(self.RelaReloc(base + addend, symbolSection, targetInstructionSection, relocType))

    def __init__(self, array_of_bytes: bytearray) -> None:
        self.bytes = array_of_bytes
        self.__moduleFileHeader = self.ModuleFileHeaderDataClass(self.bytes)


        # Validate header
        if ((self.__moduleFileHeader.formTag != b"FORM") or (self.__moduleFileHeader.moduTag != b"MODU") or (self.__moduleFileHeader.padTag != b"PAD ") or (self.__moduleFileHeader.commTag != b"COMM")):
            raise ValueError("Invalid module file header")
        
        self.text_rom_start = self.entry_offset = MODULE_FILES_CODE_BYTES_START
        self.text_rom_end = MODULE_FILES_CODE_BYTES_START + self.__moduleFileHeader.textSize
        self.rodata_rom_start = MODULE_FILES_CODE_BYTES_START + self.__moduleFileHeader.textSize
        self.rodata_rom_end = MODULE_FILES_CODE_BYTES_START + self.__moduleFileHeader.rodataSize + self.__moduleFileHeader.textSize
        self.data_rom_start = MODULE_FILES_CODE_BYTES_START + self.__moduleFileHeader.rodataSize + self.__moduleFileHeader.textSize
        self.bss_rom_start = self.code_end_offset = self.data_rom_end = MODULE_FILES_CODE_BYTES_START + self.__moduleFileHeader.dataSize + self.__moduleFileHeader.textSize + self.__moduleFileHeader.rodataSize
        self.bss_rom_end = self.bss_rom_start + self.__moduleFileHeader.bssSize
        
        self.textFakeVAddr = MODULE_FILES_TEXT_SEC_FAKE_VADDR
        self.rodataFakeVAddr = self.textFakeVAddr + self.__moduleFileHeader.textSize
        self.dataFakeVAddr = self.textFakeVAddr + self.__moduleFileHeader.textSize + self.__moduleFileHeader.rodataSize 
        self.bssFakeVAddr = self.textFakeVAddr + self.__moduleFileHeader.textSize + self.__moduleFileHeader.rodataSize + self.__moduleFileHeader.dataSize
        

        if (False):
            print(f"Text rom start: {hex(self.text_rom_start)}")
            print(f"Text rom end:  {hex(self.text_rom_end)}")
            print(f"Rodata rom start:  {hex(self.rodata_rom_start)}")
            print(f"Rodata rom end:  {hex(self.rodata_rom_end)}")
            print(f"Data rom start:  {hex(self.data_rom_start)}")
            print(f"Data rom end:  {hex(self.data_rom_end)}")


PROGNAME = "moduleFileDisasm"

def getToolDescription() -> str:
    return "Experimental Disassembler of Module files"

def addOptionsToParser(parser: argparse.ArgumentParser) -> argparse.ArgumentParser:
    parser.add_argument("-V", "--version", action="version", version=f"%(prog)s {__version__}")

    parser.add_argument("binary", help="Path to input module file")
    parser.add_argument("output", help="Path to output. Use '-' to print to stdout instead")

    parser.add_argument("--data-output", help="Path to output the data and rodata disassembly")
    parser.add_argument("--module-name", help="4 byte name of the module to disassembly")
    parser.add_argument("--file-splits", help="Path to a file splits csv")

    parser.add_argument("--split-functions", help="Enables the function and rodata splitter. Expects a path to place the splited functions", metavar="PATH")

    parser.add_argument("--instr-category", help="The instruction category to use when disassembling every passed instruction. Defaults to 'cpu'", choices=["cpu", "rsp", "r3000gte", "r4000allegrex", "r5900"])
    parser.add_argument("--function-info", help="Specifies a path where to output a csvs sumary file of every analyzed function", metavar="PATH")

    parser.add_argument("--dont-split-data", help="Don't split data", action=common.Utils.BooleanOptionalAction)
    parser.add_argument("--dont-split-bss", help="Don't split bss", action=common.Utils.BooleanOptionalAction)
    parser.add_argument("--dont-split-rodata", help="Don't split rodata", action=common.Utils.BooleanOptionalAction)
    parser.add_argument("--split-text", help="Don't split rodata", action=common.Utils.BooleanOptionalAction)

    parser.add_argument("--create-c-file", help="Write a C file", action=common.Utils.BooleanOptionalAction)
    


    common.Context.addParametersToArgParse(parser)

    common.GlobalConfig.addParametersToArgParse(parser)

    mips.InstructionConfig.addParametersToArgParse(parser)

    return parser

def getArgsParser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=getToolDescription(), prog=PROGNAME, formatter_class=common.Utils.PreserveWhiteSpaceWrapRawTextHelpFormatter)
    return addOptionsToParser(parser)

def applyArgs(args: argparse.Namespace) -> None:
    if args.libultra_syms is None:
        args.libultra_syms = False
    if args.hardware_regs is None:
        args.hardware_regs = False
    common.GlobalConfig.parseArgs(args)
    mips.InstructionConfig.parseArgs(args)

def applyGlobalConfigurations() -> None:
    common.GlobalConfig.REMOVE_POINTERS = False
    common.GlobalConfig.IGNORE_BRANCHES = False
    common.GlobalConfig.SYMBOL_FINDER_FILTER_LOW_ADDRESSES = False

    common.GlobalConfig.ALLOW_UNKSEGMENT = False

SpecialSectionNames = {".text", ".data", ".rodata", ".bss"}

def getOutputPath(inputPath: Path, textOutput: Path, dataOutput: Path, sectionType: common.FileSectionType, sectionName: str) -> Path:
    outputPath = dataOutput
    if sectionType == common.FileSectionType.Text:
        outputPath = textOutput

    outputFilePath = outputPath
    if str(outputPath) != "-":
        outname = f"{inputPath.stem}"
        if sectionName not in SpecialSectionNames:
            outname += f"_{sectionName}"
        outputFilePath = outputFilePath / outname

    return outputFilePath

def processSection(
        context: common.Context, array_of_bytes: bytes,
        processedSections: dict[common.FileSectionType, list[mips.sections.SectionBase]],
        segmentPaths: dict[common.FileSectionType, list[Path]],
        sectionsPerName: dict[str, mips.sections.SectionBase],
        inputPath: Path, textOutput: Path, dataOutput: Path,
        sectionName: str, 
        romStart: int,
        romEnd: int,
        vramStart: int,
        secSize: int,
        sectionType: common.FileSectionType,
        sectionClass: type[mips.sections.SectionText]|type[mips.sections.SectionData]|type[mips.sections.SectionRodata]|type[mips.sections.SectionBss]
    ) -> None:
    outputFilePath = getOutputPath(inputPath, textOutput, dataOutput, sectionType, sectionName)

    vromStart = romStart
    vromEnd = romEnd
    vramStart = vramStart

    mipsSection: mips.sections.SectionBase


    inname = f"{inputPath.stem}"
    if sectionName not in SpecialSectionNames:
        inname += f"_{sectionName}"

    if issubclass(sectionClass, mips.sections.SectionBss):
        bssStart = vramStart
        bssEnd = bssStart + secSize
        mipsSection = sectionClass(context, vromStart, vromEnd, bssStart, bssEnd, inname, 0, None)
    else:
        mipsSection = sectionClass(context, vromStart, vromEnd, vramStart, inname, array_of_bytes, 0, None)

    mipsSection.setCommentOffset(vromStart)
    mipsSection.customSectionName = sectionName
    processedSections[sectionType].append(mipsSection)
    segmentPaths[sectionType].append(outputFilePath)
    sectionsPerName[sectionName] = mipsSection


def getProcessedSections(context: common.Context, array_of_bytes: bytes, moduleFileParser: ModuleFileParse, inputPath: Path, textOutput: Path, dataOutput: Path) -> tuple[dict[common.FileSectionType, list[mips.sections.SectionBase]], dict[common.FileSectionType, list[Path]], dict[str, mips.sections.SectionBase]]:
    processedSections: dict[common.FileSectionType, list[mips.sections.SectionBase]] = {
        common.FileSectionType.Text: [],
        common.FileSectionType.Data: [],
        common.FileSectionType.Rodata: [],
        common.FileSectionType.Bss: [],
    }
    segmentPaths: dict[common.FileSectionType, list[Path]] = {
        common.FileSectionType.Text: [],
        common.FileSectionType.Data: [],
        common.FileSectionType.Rodata: [],
        common.FileSectionType.Bss: [],
    }
    sectionsPerName: dict[str, mips.sections.SectionBase] = dict()

   
    processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath,
                    textOutput, dataOutput, ".text", moduleFileParser.text_rom_start, moduleFileParser.text_rom_end, 
                    moduleFileParser.textFakeVAddr, moduleFileParser.getTextSize(),
                      common.FileSectionType.Text, mips.sections.SectionText)
    
    processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath,
                    textOutput, dataOutput, ".rodata", moduleFileParser.rodata_rom_start, moduleFileParser.rodata_rom_end, 
                    moduleFileParser.rodataFakeVAddr, moduleFileParser.getRodataSize(),
                      common.FileSectionType.Rodata, mips.sections.SectionRodata)
    
    processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath,
                    textOutput, dataOutput, ".data", moduleFileParser.data_rom_start, moduleFileParser.data_rom_end, 
                    moduleFileParser.dataFakeVAddr, moduleFileParser.getDataSize(),
                      common.FileSectionType.Data, mips.sections.SectionData)
    
    processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath,
                    textOutput, dataOutput, ".bss", moduleFileParser.bss_rom_start, moduleFileParser.bss_rom_end, 
                    moduleFileParser.bssFakeVAddr, moduleFileParser.getBssSize(),
                      common.FileSectionType.Bss, mips.sections.SectionBss)
    
    #processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath, textOutput, dataOutput, sectionName, common.FileSectionType.Text, mips.sections.SectionText)
    #processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath, textOutput, dataOutput, sectionName, common.FileSectionType.Data, mips.sections.SectionData)
    #processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath, textOutput, dataOutput, sectionName, common.FileSectionType.Rodata, mips.sections.SectionRodata)
    #processSection(context, array_of_bytes, processedSections, segmentPaths, sectionsPerName, inputPath, textOutput, dataOutput, sectionName, common.FileSectionType.Bss, mips.sections.SectionBss)

    return processedSections, segmentPaths, sectionsPerName
    
def readSymbolAddrsFile(filepaths):
    symNamesAndAddrs = {}

    # ensure it's always iterable
    if isinstance(filepaths, (str, Path)):
        filepaths = [filepaths]

    for filepath in filepaths:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue

                line = line.split("//")[0].strip()

                if not line:
                    continue

                name, value = line.split("=")
                name = name.strip()
                value = int(value.strip().rstrip(";"), 16)

                symNamesAndAddrs[name] = value
    #print(symNamesAndAddrs)

    return symNamesAndAddrs

def renameFunctions(processedFiles: dict[common.FileSectionType, list[mips.sections.SectionBase]], moduleName: str, entryFuncVaddr: int, args: argparse.Namespace) -> None:
    dic = readSymbolAddrsFile(args.symbol_addrs)
    for textFile in processedFiles.get(common.FileSectionType.Text, []):
        for func in textFile.symbolList:
            assert isinstance(func, mips.symbols.SymbolFunction)
            if func == common.SymbolSpecialType.jumptablelabel:
                print(func.contextSym.name)

            # Avoid renaming the entry function
            if func.getVramOffset(0) == entryFuncVaddr:
                continue
            # Don't rename the functions specified in the symbol addrs file
            if dic.get(func.contextSym.name) != func.getVramOffset(0):
                func.contextSym.name =   "func_" + moduleName + "_" + f"{hex(func.getVramOffset(0)).replace('0x', '00').upper()}"
    

def renameJumpTableLabels(processedFiles: dict[common.FileSectionType, list[mips.sections.SectionBase]], moduleName: str, entryFuncVaddr: int, args: argparse.Namespace) -> None:
    dic = readSymbolAddrsFile(args.symbol_addrs)
    for textFile in processedFiles.get(common.FileSectionType.Text, []):
        symbolSeg = textFile.getSegment()
        for contextSym in symbolSeg.symbols.values():
            if contextSym.getTypeSpecial() != common.SymbolSpecialType.jumptablelabel:
                continue

            if contextSym.name is not None and dic.get(contextSym.name) == contextSym.address:
                continue

            contextSym.name = f".L{moduleName}_{contextSym.address:08X}"


def changeGlobalSegmentRanges(context: common.Context, processedSegments: dict[common.FileSectionType, list[mips.sections.SectionBase]]) -> None:
    lowestVromStart: int|None = None
    highestVromEnd: int|None = None
    lowestVramStart: int|None = None
    highestVramEnd: int|None = None

    for subSegment in processedSegments.values():
        for section in subSegment:
            if lowestVromStart is None or section.vromStart < lowestVromStart:
                lowestVromStart = section.vromStart
            if highestVromEnd is None or section.vromEnd > highestVromEnd:
                highestVromEnd = section.vromEnd
            if lowestVramStart is None or section.vram < lowestVramStart:
                lowestVramStart = section.vram
            if highestVramEnd is None or section.vramEnd > highestVramEnd:
                highestVramEnd = section.vramEnd

    if lowestVromStart is None:
        lowestVromStart = 0x0
    if highestVromEnd is None:
        highestVromEnd = 0xFFFFFFFF
    if lowestVramStart is None:
        lowestVramStart = 0x0
    if highestVramEnd is None:
        highestVramEnd = 0xFFFFFFFF
    context.changeGlobalSegmentRanges(lowestVromStart, highestVromEnd, lowestVramStart, highestVramEnd)
    return

def writeSectionFile(processedFiles: dict[common.FileSectionType, list[mips.sections.SectionBase]], sec: str, processedFilesOutputPaths: dict[common.FileSectionType, list[Path]], processedFilesCount: int) -> None:
    common.Utils.printVerbose("Writing files...")
    i = 0
    for section, filesInSection in processedFiles.items():
        if section.name != sec:
            continue
            
        pathLists = processedFilesOutputPaths[section]
        for fileIndex, f in enumerate(filesInSection):
            filePath = pathLists[fileIndex]
            mips.FilesHandlers.writeSection(filePath, f)
            i += 1
    return

def writeSourceFile(processedFiles: dict[common.FileSectionType, list[mips.sections.SectionBase]], moduleName: str, entryFuncVaddr) -> None:
    fileName = moduleName + ".c"
    filePath: Path = Path("src/modules") / fileName
    if filePath.exists():
        print(f"Skipping {filePath}, file already exists.")
        return
    with open(filePath, "w") as f:
        for textFile in processedFiles.get(common.FileSectionType.Text, []):
            for func in textFile.symbolList:
                assert isinstance(func, mips.symbols.SymbolFunction)
                if func.getVramOffset(0) == entryFuncVaddr:
                    globalAsmFunc = "asm/us/nonmatchings/modules/" + moduleName + "/" + "__entrypoint_func_" + moduleName + "_" + f"{hex(func.getVramOffset(0)).replace('0x', '')}" + ".s"
                else:
                    globalAsmFunc = "asm/us/nonmatchings/modules/" + moduleName + "/" + "func_" + moduleName + "_" + f"{hex(func.getVramOffset(0)).replace('0x', '00').upper()}" + ".s"
                f.write(f"#pragma GLOBAL_ASM(\"{globalAsmFunc}\")\n\n")


        # Write non migrated rodata symbols
        # TODO: Enable this with args.migrate_rodata_to_functions
        #symbols = getNonMigratedSymbols(processedFiles)
        #for sym in symbols:
            #globalAsmFunc = "asm/us/nonmatchings/modules/" + moduleName + "/" + sym.getName() + ".s"
            #f.write(f"#pragma GLOBAL_ASM(\"{globalAsmFunc}\")\n\n")
            

    f.close()

def writeFunctions(processedFiles: dict[common.FileSectionType, list[mips.sections.SectionBase]], filePath: Path) -> None:
    for textFile in processedFiles.get(common.FileSectionType.Text, []):
        for func in textFile.symbolList:
            assert isinstance(func, mips.symbols.SymbolFunction)
            funcPath = filePath / (func.getName()+ ".s")
            print(f"Writing function {funcPath}")
            with funcPath.open("w") as f:
                entry = mips.FunctionRodataEntry.getEntryForFuncFromPossibleRodataSections(func, [])
                entry.writeToFile(f, writeFunction=True)

def getNonMigratedSymbols(processedSegments: dict[common.FileSectionType, list[mips.sections.SectionBase]]) -> list[mips.symbols.SymbolBase]:
    rodataSymbols: list[tuple[mips.symbols.SymbolBase, mips.symbols.SymbolFunction | None]] = []

    for section in processedSegments.get(common.FileSectionType.Rodata, []):
        for sym in section.symbolList:
            rodataSymbols.append((sym, None))

    rodataSymbolsVrams = {sym.vram for sym, _ in rodataSymbols}

    for section in processedSegments.get(common.FileSectionType.Text, []):
        for func in section.symbolList:
            assert isinstance(func, mips.symbols.SymbolFunction)

            referencedRodata = rodataSymbolsVrams & func.instrAnalyzer.referencedVrams

            for i in range(len(rodataSymbols)):
                if len(referencedRodata) == 0:
                    break

                rodataSym, funcReferencingThisSym = rodataSymbols[i]

                if rodataSym.vram not in referencedRodata:
                    continue

                referencedRodata.remove(rodataSym.vram)

                if funcReferencingThisSym is not None:
                    continue

                rodataSymbols[i] = (rodataSym, func)

    # collect the ones that couldn't be migrated
    nonMigrated: list[mips.symbols.SymbolBase] = []
    for rodataSym, funcReferencingThisSym in rodataSymbols:
        if funcReferencingThisSym is None:
            nonMigrated.append(rodataSym)

    return nonMigrated

def processArguments(args: argparse.Namespace) -> int:
    applyArgs(args)

    applyGlobalConfigurations()

    context = common.Context()
    context.parseArgs(args)

    common.Utils.printQuietless(f"{PROGNAME} (spimdisasm {__version__})")

    inputPath = Path(args.binary)
    array_of_bytes = common.Utils.readFileAsBytearray(inputPath)
    moduleFile = ModuleFileParse(array_of_bytes)

    entryFuncVaddr = MODULE_FILES_TEXT_SEC_FAKE_VADDR + moduleFile.getEntryFuncOffset()
    instrCategory = args.instr_category

    textOutput = Path(args.output)
    if args.data_output is None:
        dataOutput = textOutput
    else:
        dataOutput = Path(args.data_output)

    processedSegments, segmentPaths, sectionsPerName = getProcessedSections(context, array_of_bytes, moduleFile, inputPath, textOutput, dataOutput)

    changeGlobalSegmentRanges(context, processedSegments)

    fec.FrontendUtilities.configureProcessedFiles(processedSegments, instrCategory)

    processedFilesCount = 0
    for sect in processedSegments.values():
        processedFilesCount += len(sect)

    common.Utils.printQuietless(f"{PROGNAME} {inputPath}: Analyzing sections...")
    fec.FrontendUtilities.analyzeProcessedFiles(processedSegments, segmentPaths, processedFilesCount)

    if args.module_name is not None:
        renameFunctions(processedSegments, args.module_name, entryFuncVaddr, args)
        renameJumpTableLabels(processedSegments, args.module_name, entryFuncVaddr, args)

    for sect in processedSegments.get(common.FileSectionType.Text, []):
        symbolSeg = sect.getSegment()
        dic= symbolSeg.symbols
        print(dic.get(3))
        print("a")

    common.Utils.printQuietless(f"{PROGNAME} {inputPath}: Writing files...")

    if args.dont_split_bss is None:
        writeSectionFile(processedSegments, "Bss", segmentPaths, processedFilesCount)

    if args.dont_split_data is None:
        writeSectionFile(processedSegments, "Data", segmentPaths, processedFilesCount)

    if args.dont_split_rodata is None:
        writeSectionFile(processedSegments, "Rodata", segmentPaths, processedFilesCount)

    if args.split_text is not None:
        writeSectionFile(processedSegments, "Text", segmentPaths, processedFilesCount)

    if args.split_functions is not None:
        common.Utils.printVerbose("\nSpliting functions...")
        progressCallback = fec.FrontendUtilities.progressCallback_migrateFunctions
        fec.FrontendUtilities.migrateFunctions(processedSegments, Path(args.split_functions), progressCallback)


    if args.create_c_file is not None:
        if args.module_name is None:
            raise ValueError("You must specifiy --module-name to write a C file")
    
        writeSourceFile(processedSegments, args.module_name, entryFuncVaddr)

    if args.save_context is not None:
        common.Utils.printQuietless(f"{PROGNAME} {inputPath}: Writing context...")
        contextPath = Path(args.save_context)
        contextPath.parent.mkdir(parents=True, exist_ok=True)
        context.saveContextToFile(contextPath)

    if args.function_info is not None:
        fec.FrontendUtilities.writeFunctionInfoCsv(processedSegments, Path(args.function_info))

    common.Utils.printQuietless(f"{PROGNAME} {inputPath}: Done!")

    return 0

def addSubparser(subparser: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparser.add_parser(PROGNAME, help=getToolDescription(), formatter_class=common.Utils.PreserveWhiteSpaceWrapRawTextHelpFormatter)

    addOptionsToParser(parser)

    parser.set_defaults(func=processArguments)


def elfObjDisasmMain() -> int:
    args = getArgsParser().parse_args()

    return processArguments(args)


elfObjDisasmMain()
