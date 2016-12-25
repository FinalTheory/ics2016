SECTIONS
{
	. = 0xc0100000;
	.text : 
	{
		obj/testcase/start.o(.text)
		*(.text)
	}
}
