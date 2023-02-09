[![Build Status](https://travis-ci.org/sbuberl/px.svg)](https://travis-ci.org/sbuberl/px)
![C++ Badge](https://camo.githubusercontent.com/5470b238fcbd1a4ed9f15c650df2472ece533eb5/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f7374642d632b2b31342d626c75652e737667)
[![Open Source Helpers](https://www.codetriage.com/sbuberl/px/badges/users.svg)](https://www.codetriage.com/sbuberl/px)

# px

A work in progress programming language

It transcompiles to C. It uses [ICU4C](http://site.icu-project.org/) for its unicode support apis.

## Installation

Before trying to compile and run the program, you will need a compiler for c++
[mingw](https://www.mingw-w64.org/) will suffice.

Once the repository is cloned:

### On visual studio code

1. Install [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
2. Install [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)

### Continued

Install [ICU](https://icu.unicode.org/download)

```bash
build.sh
cd compiler/src
```

Run **PxMain.cpp** with a filename fed to compile

## Documentation

### Variables

Variable declaration follows the format below

```
Variable_Name: Data_type = value;
```

Data Types

- int8/16/32/64
- uint8/16/32/64
- float32/64
- string
- char
- bool
- void
- object
- array (data_type[size])

### Conditionals

If statements follow the format below

```
if(conditional_statement){}
else if(conditional_statement){}
else{}
```

For single if statements, brackets are not required. Break and continue are supported.

While statements follow the format below

```
while(conditional_statement){}
```

### Functions

Functions should generally be declared prior to the main function and then defined after

```
func function_name(passed_variable_name:data_type) : ret_data_type {}
```

Void-returning functions are supported

### Keywords

- abstract
- as
- break
- case
- concept
- continue
- default
- do
- else
- extern
- false
- for
- func
- if
- implementation
- interface
- module
- new
- private
- protected
- public
- ref
- return
- state
- switch
- true
- value
- while

### Example

```
module test;

func blah(x:int32, y:int32) : void;

func main() : int32
{
	blah(123, 586);
	return 27;
}

func blah(x:int32, y:int32) : void
{
	a: uint32 = 1024_u32;
	i: int32 = x + -y;
	j: int32 = i ÷ 4.0 as int32;
	0x10ADF + 0b10101001 + 0o15675;
	i = j > 12 ? 256 : 384;
	b: bool = true;
	if( b == false )
	{
		q: int32 = j * i;
		b = true;
	}
    else if( i > 384 )
    {
        w: char = 'M';
    }
	else
		b = false;

	z: int32 = 0;
	do {
        z += 1;
        if( z == 12 )
            break;
    } while ( z > 0 && z < 20 )

    while ( z > 0 || b == true ) {
       z -= 1;
       if( z == 5 )
           continue;
    }

	c: char  = '\u263A';
	s: string = "こんにち\u263Aは世界";
	array: int64[5] = [1_i64, 3, 5, 7, 9];
	third: int64 = array[3];
	array[4] = 128_i64;
	printInt(third);
```

## Contributions

For contributing to the project, refer to [first-time contribution guide](https://github.com/firstcontributions/first-contributions) for guidance.

## License

[MIT](https://choosealicense.com/licenses/mit/)
