# Dielectric Properties Of Magnetic Fluid

## Description

The gui application for estimating the hydrodynamic dimensions of ferrofluid particles based on the analysis of their dielectric properties.

This program is designed to process data obtained in the study of magnetic fluid.

Usually, in all physical problems related to the diffusion or viscosity of a magnetic fluid (MF), the hydrodynamic particle size of the MF is required. One of the methods capable of measuring precisely the hydrodynamic size can be dielectric analysis, i.e. measurement of frequency dependences of complex permittivity. According to the Debye theory, the relaxation time of particles in an alternating electric field is determined by the effective (hydrodynamic) particle size.

### Debye formulas
According to the Debye theory, the real and imaginary parts of the complex permittivity $\varepsilon = \varepsilon^{'} + i \cdot \varepsilon^{''}$ are expressed as:

$$ \varepsilon^{'} = \varepsilon_{\infty} + \dfrac{\varepsilon - \varepsilon_{\infty}}{1 + \omega^2 \tau^2},$$

$$ \varepsilon^{''} = \dfrac{(\varepsilon - \varepsilon_{\infty}) \cdot \omega \tau_D}{1 + \omega^2 \tau^2}.$$

Here $\varepsilon$ $-$ is the permittivity in a constant field,  $\varepsilon_{\infty}$ $-$ the permittivity at a frequency tending to infinity, $\tau_D$ $-$ the relaxation time and $\omega$ $-$ the cyclic frequency.