#include <Format.h>
#include <stdlib.h>
#include <stdio.h>

void FormatPrint(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	size_t len = Format(NULL, 0, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);

	char *str = malloc(len + 1);
	str[len] = 0;

	len = Format(str, len, fmt, ap);

	va_end(ap);

	printf("%s", str);

	free(str);
}

int main()
{
	FormatPrint("%{ 20%s\n", "St. Mary");
	FormatPrint("%{ i%s\n", 20, "St. Mary");
	FormatPrint("%{ci%s\n", ' ', 20, "St. Mary");
}
