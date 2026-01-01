# KiCAS2-Numeric

The goal of this repo is to provide utility functions for interacting with numbers used by the KiCAS2 system. Provided functions include:

* Parse rational numbers from the string format `['0'-'9']* ('.' ['0'-'9']*)? ('e' ['+'|'-']? ['0'-'9']*)?`
* Append numbers to the end of a string
* Miscellaneous functions for word-sized mathematical operations, which are useful if they outperform Flint in benchmarks
* Debug allocations tracking to ensure all GMP/Flint allocated memory is freed

A secondary goal is to work through an example of how to dynamically link against the LGPL-licensed libraries GMP and Flint while using CI against various targets. Dynamic linking is important for digital rights compliance given the terms of the LGPL.

## License

This example repo is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.