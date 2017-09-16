## About

`jsonc` is a small C program that can convert free-form JSON
data into a set of rich API docs.

For instance, it can turn a file like this:
```
//struct Unit
{
	name: "John", //name of the unit
	tags: [ "grunt", "human" ],
	weapon:
	//struct Weapon
	{
		"name": "Magnum",
		"ammo": 9,
	} 
}
```
into a set of tables like this:

__Unit__
struct Unit

| Member | Type             | Description       |
|--------|------------------|-------------------|
| name   | String           | name of the unit  |
| tags   | Array [ String ] |                   |
| weapon | Weapon           |                   |
-------------------------------------------------

__Weapon__
struct Weapon

| Member | Type   | Description |
|--------|--------|-------------|
| name   | String |             |
| ammo   | Number |             |
---------------------------------

`jsonc` can be used both stand-alone and as a filter in
some other pipeline.

Currently supported output formats are:

- HTML (fragments and full pages)
- XML
- Markdown
- Doxygen C (see below)
- ...and a bunch of CLI-tailored text formats

### Building and Running

You need `gcc` toolchain, `flex` and `bison`. Type `make`.

To install, use `make install`.

See `jsonc --help` for usage instructions. In general,
you might want to do something like:

```
jsonc input.json --html > output.html
```

## Description

`jsonc` treats input JSON like it's JavaScript, meaning:

- Both single-quoted and double-quoted string are allowed
- Unqouted object keys are allowed
- Literal Unicode in string values is allowed
- Trailing commas in element listings are allowed
- Multi-line and single-line comments are allowed (and expected!)

`jsonc` expects to find annotations in the form of
 `struct (\w+)` before each struct, for example:
```
//struct Item
{
	'name': 'something',
}
```

`jsonc` will look for word "optional" in struct member
 annotations, e.g.:

```
//struct Item
{
	'name': 'something', //optional.
}
```

For struct member annotations, `jsonc` can accept both heading --

```
{
	//about name
	'name': '',
	//about title
	'title': '',
}
```

-- and trailing --

```
{
	'name': '',  //about name
	'title': '', //about title
}
```

-- comments, but you can't use both in the same struct!

## Output formats

### HTML

Command:
- `--html`

Options:
- `--page` (default) Output whole HTML page.
- `--fragment` Output only a fragment.

### CSS only

Command:
- `--css`

If you need the default/built-in CSS for some reason.

### XML

Command:
- `--xml`

Generates abstract XML, suitable for further transforms.

### Markdown

Command:
- `--md`

TODO: prettier tables

### Doxygen C

Command

- `--doxygen`

Generates some C-looking code with javadoc comments,
which could be fed into doxygen.

To embed `jsonc` into an existing project, you will need
the following.

1. `json-doxygen` script, which is a simple wrapper:

```
#!/bin/sh

./jsonc --doxygen ${1}
```

2. In your `Doxyfile`, add that script to your filters:

```
FILTER_PATTERNS = *.json=./jsonc-doxgyen
```

3. In your `Doxyfile`, somehow add the json files into input, i.e.:

```
INPUT = lib1.hpp src1.cpp my_api_file.json
```

See `doxygen` directory in `jsonc` source distribution
for an example skeleton project. The `Doxyfile` used there
is identical to a default one, except for `FILTER_PATTERNS`
and `INPUT` directives.

## License

The lex/yacc parts of `jsonc` were heavily based on this
(https://gist.github.com/justjkk/436828)[gist], therefore,
it is licensed under The MIT License (MIT) just the same.

See `LICENSE` file for more info.
