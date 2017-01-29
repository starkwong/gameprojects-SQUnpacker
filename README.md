# gameprojects-SQUnpacker

SQUnpacker is a tool to unpack resource files from Android version of Sister Quest RPG (大進撃RPG！シスタークエスト) mobile game,
which has been closed on 2015 after serving for roughly 1.5 year.

Unpacking of downloaded resource is fully worked, while unpacking of bundled resource only works for png, wav and mp3 files.

### Basic Information

**Project Language**: Visual C
**Project Version**: Visual Studio 2010
**Archive Format**: Unity 4 Bundle
**Unity Version**: 4.6.5f1
**Compression**: None
**Encryption**: None

### Usage

To use SQUnpacker, first prepare the required resource files:
* Downloaded Resources: Retrieve from /sdcard/Android/data/com.snkplaymore.android011/files, put it as ./files (Direct run from VS) or ./Debug/files (Run after building)
* Bundled Resources: Extract from asset folder in apk, put it as ./Data

Then either run directly from VS or build and run, outputs will be placed in output (Downloaded) and output2 (Bundled). Missing resource directories will be ignored.

### Technical Information (Blackbox)

Downloaded Resources:

...|A|B|C...|D...|E|F|G...|

* Starts from 0x1074
* A: [DWORD] Number of entries in bundle
* B: [QDWORD] Number of unknown records
* C: [B*DWORD] Unknown records
* D: [A*s1] Entry descriptors
* s1: AA: [DWORD] Length of entry name
* s1: AB: [AA round up to 4 bytes] Entry name
* s1: AC: [16] Unknown
* E: [16 if first byte is 0x01]
* E: [24 if first byte is 0x02]
* F: [4 if first byte is 0x00]
* G: [A*s2] Entry content
* s2: BA: [DWORD] Length of entry name
* s2: BB: [BA round up to 4 bytes] Entry name
* s2: BC: [DWORD] Length of content
* s2: BD: [s3 if BC == 2]
* s3: CA: [12] Unknown
* s3: CB: [BC=DWORD] Correct length of content
* s2: BD: [s4 if BC == 0x00000200 && current bytes = 0x00000080] Exception case 1
* s4: DA: [48] Unknown
* s4: DB: [BC=DWORD] Correct length of content
* s2: BD: [s4 if BC == 0x00000400 && current bytes = 0x00000100] Exception case 2
* s2: BE: [BC] Content
* s2: BF: [* Until current byte != 0x00]


Bundled Resources:

...|A|...|s1|

* Starts from 0x3
* A: [BYTE} Detection flag
* B: [s1 if　A == 0x31] Files we are able to extract
* s1: Starts from 0x1000
* s1: AA: [DWORD] Length of entry name
* s1: AB: [AA round up to 4 bytes] Entry name
* s1: AC: [DWORD] Length of content
* s1: AD: [s2 if first byte == 0x02 && current bytes + 4 == 0x0000000d] Unknown
* s2: BA: [16] Unknown
* s2: BB: [AC=DWORD] Correct length of content
* s1: AE: [4] Unknown
* s1: AF: [AC] Content
