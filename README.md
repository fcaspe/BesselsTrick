
# Bessel's Trick
### FM Tone Transfer Plugin using Neural Nets

[GITHUB HEADER DOWNLOAD LATEST BUILD!]

Bessel's Trick is a Neural Audio Plugin for Timbre Transformation of monophonic audio signals using an FM synthesizer. 

The main characteristics are:
 - Synth controllable from an Audio Input
    - Try feedng it voice, violin, solo guitar, trumpet.
 - Designed for **live playing** 
    - Fast sound generation, *1.3ms inference latency*.
 - 6-oscillator FM engine
    - Based on the classic Yamaha DX7.
 - Comes with many pretrained examples!
    - See the Release webpage.
 - Tweakable FM controls on real time.
 - Can train your own model.

## How does it work?

### Why FM?

### Why Neural Nets?

## Train your own model

This plugin supports models trained with the [Envelope Learning](https://qmro.qmul.ac.uk/xmlui/handle/123456789/90812) method.  
For info on how to train new models visit the [fmtransfer](https://github.com/fcaspe/fmtransfer) repository.
## Build it

Clone this repository and run:

```bash
source build.sh
```

This will run the build and install process.

## Acknowledgements
 - Jeff Miller (GUI design tips)
 - Rodrigo Diaz (Building Pipeline, Apple Notarization)
 - Daniel Walz (PGM library tips)

## Licenses
 - Bessel's Trick released under BSD-3.
 - Foleys GUI Magic Library licensed under BSD-3.
 - YIN Pitch Tracker adapted from [Pidato](https://github.com/JorenSix/Pidato), licensed under GNU GPL 3.

 ## Citation
 If you find this work useful, please consider citing us:

```bibtex
@article{caspe2023learnedenvelopes,
    title={{FM Tone Transfer with Learned Envelopes}},
    author={Caspe, Franco and McPherson, Andrew and Sandler, Mark},
    journal={Proceedings of Audio Mostly 2023},
    year={2023}
}
```