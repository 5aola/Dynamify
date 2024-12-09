# Dynamify
A 24-band dynamics VST3 plugin was developed as my Bachelor thesis at the Budapest University of Technology and Economics, under the supervision of the Laboratory of Acoustics and Studio Technologies.

The following dynamics processors are implemented:

* Dawnwards Compressor (basic)
* Upwards Compressor
* All-direction Compressor (OTT)
* Limiter
* Expander
* Gate

## Brief summary of my thesis

My thesis focused on the implementation of real-time multiband dynamics processing effects. The aim was to enhance these effects by aligning them more closely with our perceptual hearing, providing greater flexibility through frequency-dependent processing.

First, I divided the frequency spectrum into 24 separate bands, drawing inspiration from the Bark model of 24 critical bands, which I was already somewhat familiar with. This process turned out to be far more complex than I initially expected, as I needed to minimize phase cancellation issues when summing up all the separate bands. Ultimately I used Linkwitz-Riley filter designs for the filter-bank.

After I implemented the dynamic processors, including a downwards and upwards compressor, a limiter, an expander, and a gate effect, with real-time adjustable parameters such as threshold, ratio, attack, and release time. Here an accurate loundess metering was crucial.

By combining the filter-bank with individual dynamics processing and and designing a graphical interface, I developed fully functioning VST software, which was sufficient for a perfect grade for my thesis.

### Used external libraries

- [JUCE Framework](https://docs.juce.com/master/index.html)
- [VST3 SDK](https://steinbergmedia.github.io/vst3_doc/vstsdk/index.html)
- [Vinnie Falco DSP Filters](https://github.com/vinniefalco/DSPFilters)
- [FFTW](https://www.fftw.org/fftw3.pdf)




## Graphical Interface

* All the yellow elements can be changed real-time. 
* The grey is the input and the blue is the output spectrum.

<img width="688" alt="Screenshot 2024-12-09 at 12 50 33" src="https://github.com/user-attachments/assets/15c765ba-a868-41be-ab4a-2d1f13496a7d">



