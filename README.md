[![Build Status](https://travis-ci.org/sbuberl/px.svg)](https://travis-ci.org/sbuberl/px)
![C++ Badge](https://camo.githubusercontent.com/5470b238fcbd1a4ed9f15c650df2472ece533eb5/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f7374642d632b2b31342d626c75652e737667)
[![Open Source Helpers](https://www.codetriage.com/sbuberl/px/badges/users.svg)](https://www.codetriage.com/sbuberl/px)

# px
A work in progress programming language

It transcompiles to C.  It uses [ICU4C](http://site.icu-project.org/) for its unicode support apis.

### Code example of working features:
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
