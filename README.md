# PSIORI ACT (PSI-ACT)

A framework for pragmatic decision making in cognitive architectures.

The doxygen documentation is available at [psiact.azurewebsites.net](https://psiact.azurewebsites.net).

This project is licensed under the BSD 3-Clause License – see the [LICENSE](LICENSE) file for details.

# Install

## Build psi-act library

Create a build-directory for storing configuration and temporary files:

    > mkdir build

Change to directory:

    > cd build

Configure cmake and create make files (also builds the tools):

    > cmake -DCMAKE_BUILD_TYPE=Release ..

Build and install psi-act:

    > make
    > make install

This will "install" the library within the package at lib/. In order to install the library system wide you need to set the install path before running make install:

    > cmake -DCMAKE_INSTALL_PREFIX=/opt/local/ ../
    > make
    > sudo make install

Create documentation from sources:

    > make doc

The documentation can be found in
doc/html/index.html

You can create a fresh XCode project from scratch from the files and use
this to edit and build psi-act. Just run cmake's generator:

    > mkdir build_xcode
    > cd build_xcode
    > cmake -G Xcode ..

This will generate a new project psi-act.xcodeproj.

# Origins and Further Reading

PSI-ACT is a non-dogmatic blend of several "classical" ideas from cognitive architecture 
research with some -- at that time -- newer ideas from the AI community playing robotic
soccer. 

It mixes ideas from 
- Rodney Brook's (former Director of MIT Artificial Intelligence Laboratory and founder
  of iRobot) subsumption architecture, which is purely reactive, 
- with ideas of traditional Belief Desire Intention (BDI) Architecture such as described in Rao
  and Georgeff, BDI Agents: From Theory to Practice and implemented in the OASIS airtraffic
  control system,
- with the hierarchical structuring of complex "plans" first implemented in the Brainstormers
  Tribots 2D software league team's behaviour architecture
- together with the idea of reasoning on "predicted beliefs" which was inspired by a paper
  of Roul Rojas group, Predicting away Robot Control Latency.

The architecture's basis was designed by Sascha Lange in 2003 / 2004, for the first time presented
during his first working week in spring 2004 and subsequently implemented inside the
robot control software of the Brainstormers Tribots afterwards.

Unfortunately, although quite successul in enabling the implementation of complex, 
distributed real-time strategies in the team of 6 soccer robots, the architecture and 
principles were never fully published. Only some aspects were detailed in Lauer et al, 
Cognitive Concepts in Autonomous Soccer Playing Robots and, Riedmiller et al,
Reinforcement Learning for Robot Soccer.

This library is a complete rewrite of the architecture, learning from earlier mistakes and
unnecessary complexity, specifically removing the confusing differentiation between
Skills and Behaviors that was implemented in the Tribot's version, and adding a module
for explicit deliberation about goals and desires.

All mentioned papers can be found in 

    /doc/literature

# Authors

- Sascha Lange <sascha@psiori.com>
