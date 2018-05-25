![C++ Badge](https://camo.githubusercontent.com/5470b238fcbd1a4ed9f15c650df2472ece533eb5/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f7374642d632b2b31342d626c75652e737667)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ab463a33c64b4bd3abd9d3c83768b2b5)](https://www.codacy.com/app/sbuberl/px?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sbuberl/px&amp;utm_campaign=Badge_Grade)

# px
A work in progress programming language

It uses [LLVM](https://llvm.org/) for its backend and [ICU4C](http://site.icu-project.org/) for its unicode supprt apis.

### Code example of working features:
```
func blah() : void
{
	a: uint32 = 1024_u32;
	i: int32 = 7_i;
	j: int32 = i ÷ 4.0 as int32;
	0x10ADF + 0b10101001 + 0o15675;
	i = j > 12 ? 256 : 384;
	b: bool = true;
	if( b == false )
	{
		q: int32 = j * i;
		b = true;
	}
	else
		b = false;
	c: char  = 'A';
	s: string = "こんにちは世界";
}

func main() : int32
{
	return 27;
}
```
