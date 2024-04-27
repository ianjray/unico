# unico
Unit Conversion

## Usage

```shell
$ unico 6\'3\" m 6\'3\" ft 6\'3\" \'\"
6.25 ft is 1.905 m
6.25 ft is 6.25 ft
6.25 ft is 6 ' 3 "

$ unico 1 pt ml
1 pt is 568.261 ml

$ unico 1 m^3 l 2 m^3 L
1 m^3 is 1000 l
2 m^3 is 2000 L

$ unico 7 lbs 14 oz kg
7.875 lb is 3.57204 kg
```

## Supported Units

```
Length.
	mm, millimetre, millimetres, millimeter, millimeters
	cm, centimetre, centimetres, centimeter, centimeters
	m, metre, metres, meter, meters
	km, kilometre, kilometres, kilometer, kilometers
	mi, mile, miles
	yd, yds, yard, yards
	', ft, feet, foot
	'"
	", in, inch, inches
Area.
	m^2
	ha, hectare
	acre, acres
	ft^2, sq ft, square foot, square feet
Volume.
	m^3
    cm^3, cc
	mL
	cL
	dL
	L
	ml
	cl
	dl
	l
	pt, pint
	US pt, US pint
	ft^3, cu ft, cubic foot, cubic feet
	in^3, cu in, cubic inch
Mass.
	mg, milligram, milligrams
	g, gram, grams
	kg, kilogram, kilograms
	Mg, megagram, megagrams
	t, tonne, tonnes
	tn, short ton, short tons, US ton, US tons
	long ton, long tons, imperial ton, imperial tons
	lb, lbs, pound, pounds
	oz, ounce, ounces
Thermodynamic temperature.
	K, kelvin
	°C, 'C, degree Celsius, degrees Celsius
	°F, 'F, degree Fahrenheit, degrees Fahrenheit
Pressure.
	Pa, pascal
	hPa, hectopascal, hectopascals
	kPa, kilopascal, kilopascals
	mbar, millibar, millibars
	bar
	psi
	mmHg, mm Hg
	inHg, "Hg
Plane angle.
	rad, radian, radians
	°, degree, degrees
```

# Code Generation Notes

A macro file [unit.hi](unit.hi) is used to describe units and the relationship to base units.

Files [unit.head.c](unit.head.c) and [unit.body.c](unit.body.c) are preprocessed and used to create `unit.c`.
This approach is used to make code coverage checking work nicely with the macro expansions.
