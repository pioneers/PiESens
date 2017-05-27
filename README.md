# PiE Electrical

This is the working repo for PiE Electrical teams

```
(c) Pioneers In Engineering
Design by: YOUR_NAMES_HERE
This design is open source hardware.
For more information, visit:
https://github.com/pioneers/PiESens
```

The revision number should be REV 10A for Revision A Year 10

Arduino Pro Micro Pinouts:

![Pinouts](https://cdn.sparkfun.com/assets/9/c/3/c/4/523a1765757b7f5c6e8b4567.png)

## How to Create a Gerber

1. Open the brd file
2. Click Cam Job
3. Open pie.cam
4. Make a directory in each sensor's folder called SENSOR\_NAME-gerb
    * SENSOR\_NAME **must** be the exact name as the brd file
5. Run job, and click yes to all for the polygon issue
6. Once all the jobs are done, run `make gerb`


## Git Work Flow

Protocol when working on Eagle files

[PiE Git Presentation Tutorial](https://docs.google.com/a/pioneers.berkeley.edu/presentation/d/1WO-AD3cTi1QdKW15F8ecN9V1b8u3yIKzHzLGQXDs-uM/edit?usp=sharing) for your own reference

**WARNING**: Do not modify pie.lbr, pie.dru, or pie.cam without discussing it with PMs 

### Initial Setup of Electrical repo on your computer

1. Copy the URL for pioneers/SmartSensors to your clipboard: (Or here: https://github.com/pioneers/SmartSensors.git)
2. In Terminal (mac) or GitBash (windows), navigate to your working directory (that is, the directory that you want to keep SmartSensors in.)
3. Clone SmartSensors by running this command: "git clone https://github.com/pioneers/SmartSensors.git "
4.If you haven't started your sub project: git checkout -b YOUR\_NAME/SUBPROJECT
  4.. YOUR\_NAME/SUBPROJECT is now the name of your branch
5. Do your work in EAGLE.


### Steps when working on EAGLE files:

1. In Terminal (mac) or GitBash (windows), navigate to your working directory (wherever you have SmartSensors).
2. Make sure your working directory is clean: git status
3. If you haven't started your sub project: git checkout -b YOUR\_NAME/SUBPROJECT
  3. YOUR\_NAME/SUBPROJECT is now the name of your branch
4. Do your work in EAGLE.
5. Once you are at a good stopping point in your EAGLE work, commit your progress:
  5. git add path/to/your/file
  5. git commit -m "Make your commit message short and with this format" 
  5. **NOTE**: Commiting is like saving your file, but for all of PiE as well as your personal computer. And just like saving, Commit Early & Commit Often!
6. Repeat Steps 4 and 5 until you are finished with your EAGLE work.
7. Once you have made your last commit, push your files to the Smartsensors github repo: git push origin YOUR\_BRANCH
    * If you get a merge conflict, contact the PMs for help.
8. If **completely** done, submit a Pull Request


### Generating Docu Packets

Make sure that `eagle xvfb pdftk` are all installed.

If you don't have the proper directories loaded into Eagle, the first script will hang forever. Both should take only a moment.

The `Docu/all_docu.sh` script will run the following for all boards:

```shell
python generate_bom.py ../Boards/BOARD_NAME/BOARD_NAME.sch BOARD_NAME.csv

python docu-packet-gen.py ../Boards/BOARD_NAME/BOARD_NAME.sch ../Boards/BOARD_NAME/BOARD_NAME.brd BOARD_NAME.csv BOARD_NAME.pdf
```

### FAQ

Q: What's the difference between commit before I pull?

A: Do you want to keep any of the changes? Then commit those (the ones you want to keep) before you pull. Or else, the things you pull might conflict with your work, and you'll have a git disaster.

Q: I worked on my stuff, pulled, and everything I did went away!

A: As long as you committed, the PMs can recover your work for you.

Q: Why am I working on my own branch?

A: This is to keep track of pull request and board reviews on a per board basis

### Commit Message Format Conventions

* Present Tense ("Reroute power traces on team flag" -- NOT "Rerouted power traces on team flag")
* No period at the end: ("Reroute power traces on team flag" -- NOT "Reroute power traces on team flag.")
* Keep your message length to about one sentence.

### Merge Conflicts: What Do??

Don't try to fix it yourself: Eagle files are finicky and you might break all your work!

Contact Seiya or Tobin: seiyaono@pioneers.berkeley.edu or tobinholcomb@pioneers.berkeley.edu

## Standards

Vias: 0.086 inch Diameter, 0.03937008 inch Drill

Standard Pin Distance: 0.1 inch
