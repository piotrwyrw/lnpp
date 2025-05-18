#include <cups/cups.h>
#include <cups/backend.h>

int main(int argc, char **argv)
{
	cupsBackendReport("", "", "LNPPD",
			"Standard LNPPD Device",
			"MFG:LNPPD;MDL:LNPPD;CMD:PCL,PJL;CLS:PRINTER;",
			"LNPPD Network");
	return 0;
}
