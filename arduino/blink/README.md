# Comparison of driving modes

This sketch cycles a piece of LPD8806 RGB LED-strip between its minimum and
maximum brightness levels. It does so in two ways:

1. Writing the same value from the lightness lookup table (LUT) to all four
   outputs of the strip. This provides 7-bit granularity, which is provided
   by the strip itself.
2. Staggered value output. This provides additional levels by grouping
   multiple outputs into a 'virtual output' and staggering the ramp-up of
   each individual LED in the output.

The length of strip addressed is equal to the number of outputs * 4.