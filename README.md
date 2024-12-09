# Dynamify
A multiband dynamics plugin made for my Bachelor thesis.

## Abstract

In today's world, programs are increasingly expected to do everything at the touch
of a button, and audio effects are no different. A more sophisticated implementation of
dynamics control plugins is their multiband implementation, whose main purpose is to
modify only the given frequency range. My thesis studies the topic of dynamics control
plug-ins, in particular with multiband signal processing.


First, the basic operation and parameterization of dynamics controllers is
presented. Then, effect types and different implementation options are described. Next, I
will investigate the optimal filter design for 24-band separation using MATLAB, in which
the advantages and disadvantages of the basic filter models will be presented. I have used
Linkwitz-Riley's filter design, which can be implemented by cascading two Butterworth
filters with the same parameterization. The final implementation of my software is a VST
plugin, so I will also present the development environment of the VST SDK and the
implementation of processing in it. A sufficiently intuitive graphical interface is also
important to me, so I created a spectrum using an FFT algorithm to help the user to analyse
it properly. I will then test the implemented plugin and compare it to the previously laid
out expectations. Finally, I examine the working principles and differences of some
similar software, comparing them to the tool I implemented.
