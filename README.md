# PSIORI ACT (P-ACT)

A framework for pragmatic decision making in cognitive architectures.

The doxygen documentation is available at [psiact.azurewebsites.net](https://psiact.azurewebsites.net).

# Install

## Build psi-act library

Create a build-directory for storing configuration and temporary files:

    > mkdir build

Change to directory:

    > cd build

Configure cmake and create make files (also builds the tools):

    > cmake -DCMAKE_BUILD_TYPE=Release ..

Build and install BrickTrack:

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

P-ACT is a non-dogmatic blend of several "classical" ideas from cognitive architecture 
research with some-- at that time-- newer ideas from the AI community playing robotic
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

The architecture's basis was designed by SL in 2003 / 2004, for the first time presented
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


# (Presently) crane-specific implementation notes

Hey, I implemented a stub for the "deliberation" in psi-act. Please have a look at this demo here https://github.com/psiori/psi-act/blob/deliberation/examples/fsm/main.cpp and the declaration & definition of the finite state machine. This is how I would like to implement the high level deliberation;  as a simple state machine, states represented by strings, probably 5-7 (unload truck, place on belt, etc) + start, stop and safety / idle nodes, transition conditions as functors that do calculations on the world model. The definition of these states and the changes among them would be all in one single file, together, even if probably a little bit longer (complex checks on world model can be moved out into "Predicates").

Reason for this is, I would feel better, if this high-level process representation is kept together in one place, so it's easier to keep consistency and notice side effects of later changes. Furthermore, this process definition should be quite simple and "reviewable", thus, better one place and simple, explicit structure, instead of distributed, hierarchy of behaviours.
So, this is the "desire" level, deliberating over the present process goals, the existing arbitration level with individual options / behaviors represents the "intention" level and is about implementing the plans to achieve these goals and selecting the right option for the present desire (goal) and state of the world (belief).
Thus, the arbitrators and options would then react to the present selected state ("desire") in their invocation and commitment conditions, as an additional computation to (probably) existing checks on the world model, which would make the logic there way easier. Individual arbitrators would only have to care about "a plan" to achieve this single desire and do not have to keep in mind (that much) when to transition into another process step.
This would make the deliberation about when to switch into what process step (unloading, storing, unstoring, belt placement) way more visible, centralized and not leave it implicitly distributed over the invocation conditions of the options and arbitrators. 


My idea is, that we'll have one high level priority arbitrator, that has individual arbitrators as options, probably one per possible desire (plus safety, emergency, default etc).

What we have to make sure is, that on this highest level the switches between options in reaction to changes in desire are reasonable, that is, only, if present crane state allows it. Probably we have to use a lot of "interruptable = false" here.

So, for the other, presently empty classes in /src/deliberation: Deliberator is the counter part for the top-level option (the strategy) on the intention / behavior level that we already have implemented. There will be one Deliberator in the crane AI, it will be owned by the ControlLoop, deliberate once per cycle, before we then let the strategy do its calculations. Either the options will be able to access the Deliberator as singleton or I will pass the Deliberator during the construction down to them.

For the crane, I will prepare an implementation of the deliberator using the FiniteStateMachine in the FiniteStateDeliberation class. Generic implementation will be in psi act, actual conditions will be filled in the autocrane-core, like demonstrated in examples/main.cpp. The Deliberator will set out exactly one desire / goal at a time. Good enough for the crane, I hope. Future, more complex systems, could have multiple goals and / or deliberators and could also use different mechanics than finite state machine (e.g. if-then-else, or a horn clause solver). 
    


# Authors

- Sascha Lange <sascha@psiori.com>


# Technical overview

<img src="../images/psi-act.svg" style="max-width:100%; object-fit: contain;">

The following prose aims to outline the psi-act repository on a more or less high level. For a more low level understanding of the single classes and functions, please refer to the docstrings.

The repository can be divided into three higher level parts represented by the directory structure:

* [Base classes](#base-classes)
* [Arbitration](#arbitration)
* [Deliberation](#deliberation)
