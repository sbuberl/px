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
