# Amazing Project - CS50, Winter 2016
Authors: Kyu Kim, Alan Lu, Donald Wilson

The project files are arranged in the following directory structure:

Lambrusco

├── bin<br>
│   ├── amazing_client<br>
│   └── AMStartup<br>
├── help<br>
│   ├── amazing_help.txt<br>
│   └── AMStartup_help.txt<br>
├── kkim96-project-design.pdf<br>
├── Makefile<br>
├── README.md<br>
├── results<br>
│   ├── [logs from successful trials]<br>
│   └── Cleanup.sh<br>
├── src<br>
│   ├── amazing_client.c<br>
│   ├── amazing.h<br>
│   ├── AMStartup.c<br>
│   ├── AMStartup.h<br>
│   ├── utils.c<br>
│   └── utils.h<br>
└── testing<br>
    ├── BATS.log<br>
    └── BATS.sh<br>

Notes:

Compilation and testing:
	The /testing/ directory contains a script, BATS.sh, that will make clean, build the executables, and perform argument checking for AMStartup.  (Additionally, it checks the --help functionality for both amazing_client and AMStartup.)  BATS.sh generates alog file in the same directory called BATS.log.  This log file is overwritten each timeBATS.sh is run.Alternatively, the executables can be compiled using the Makefile in the top directory.

Results logs:
	Every call to AMStartup will generate a results log in the /results/ directory.  The script /results/Cleanup.sh will remove any unsuccessful logs from the results directory.  Note that a call to "make clean" in the top directory will automatically call Cleanup.sh.
