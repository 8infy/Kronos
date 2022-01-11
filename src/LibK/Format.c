#include <Format.h>
#include <Macros.h>
#include <String.h>


#define DIGIT(n) ((n) > 9 ? ((n) - 10) + 'A' : (n) + '0')


static inline void FmtPut(struct FormatCtx *ctx, char ch)
{
	if(ctx->spos < ctx->size)
		ctx->str[ctx->spos] = ch;

	ctx->spos++;
}

static inline void FmtWrite(struct FormatCtx *ctx, const char *str, size_t count)
{
	memcpy(&ctx->str[ctx->spos], str, ctx->spos > ctx->size ? 0 : min(count, ctx->size - ctx->spos));

	ctx->spos += count;
}

static inline char FmtGet(struct FormatCtx *ctx)
{
	if(*ctx->fmt)
		return *(ctx->fmt++);

	return 0;
}

static inline void FmtPad(struct FormatCtx *ctx, struct FormatSpec *s, size_t count)
{
	if(count > s->pad)
		return;

	size_t pad = s->pad - count;

	while(pad) {
		FmtPut(ctx, s->padch);
		pad--;
	}
}

static void FmtWriteDec(struct FormatCtx *ctx, struct FormatSpec *s, uint64_t n, int neg)
{
	uint64_t r = 0;
	int digits = neg ? 1 : 0;

	while(n) {
		r  = r * 10 + (n % 10);
		n /= 10;

		digits++;
	}

	if(!s->front)
		FmtPad(ctx, s, digits);

	if(neg)
		FmtPut(ctx, '-');

	while(r) {
		int digit = r % 10;
		r /= 10;

		FmtPut(ctx, DIGIT(digit));
	}

	if(s->front)
		FmtPad(ctx, s, digits);
}

static void FmtWriteHex(struct FormatCtx *ctx, struct FormatSpec *s, uint64_t n)
{
	int digits = (2ULL << s->type);

	if(!s->front)
		FmtPad(ctx, s, digits);

	for(int i = (digits - 1); i >= 0; i--) {
		int digit = (n >> (i << 2)) & 0x0F;

		FmtPut(ctx, DIGIT(digit));
	}

	if(s->front)
		FmtPad(ctx, s, digits);
}

static void FmtWriteBin(struct FormatCtx *ctx, struct FormatSpec *s, uint64_t n)
{
	int digits = (8ULL << s->type) + (2ULL << s->type) - 1;

	if(!s->front)
		FmtPad(ctx, s, digits);

	for(int i = (8ULL << s->type) - 1; i >= 0; i--) {
		FmtPut(ctx, ((n >> i) & 1) + '0');

		if(i % 4 == 0 && i != 0)
			FmtPut(ctx, '_');
	}

	if(s->front)
		FmtPad(ctx, s, digits);
}

static void FmtWriteString(struct FormatCtx *ctx, struct FormatSpec *s, const char *str)
{
	str = str == NULL ? "(null)" : str;

	size_t len = strlen(str);

	if(!s->front)
		FmtPad(ctx, s, len);

	FmtWrite(ctx, str, len);

	if(s->front)
		FmtPad(ctx, s, len);
}

static void FmtWriteChar(struct FormatCtx *ctx, struct FormatSpec *s, char ch)
{
	if(!s->front)
		FmtPad(ctx, s, 1);

	FmtPut(ctx, ch);

	if(s->front)
		FmtPad(ctx, s, 1);
}


static inline int FormatType(char ch)
{
	switch(ch)
	{
	case 'b': return 0;
	case 's': return 1;
	case 'i': return 2;
	case 'l': return 3;
	default:  return 2;
	}
}

static void FormatExpr(struct FormatCtx *ctx, struct FormatSpec *sp, va_list ap)
{
	char ch = FmtGet(ctx);

	switch(ch)
	{
	case '%':
		break;
	case 'c':
		FmtWriteChar(ctx, sp, va_arg(ap, uint32_t));
		break;
	case 'l': {
		int64_t num = va_arg(ap, int64_t);
		int neg     = 0;

		if(num < 0) {
			num = -num;
			neg = 1;
		}

		

		FmtWriteDec(ctx, sp, num, neg);
		break;
	  }
	case 'u':
		FmtWriteDec(ctx, sp, va_arg(ap, uint64_t), 0);
		break;
	case 'x': {
		ch       = FmtGet(ctx);
		sp->type = FormatType(ch);

		uint64_t num = sp->type == 3 ? va_arg(ap, uint64_t) : va_arg(ap, uint32_t);
		FmtWriteHex(ctx, sp, num);
		break;
	  }
	case 'b': {
		ch       = FmtGet(ctx);
		sp->type = FormatType(ch);

		uint64_t num = sp->type == 3 ? va_arg(ap, uint64_t) : va_arg(ap, uint32_t);
		FmtWriteBin(ctx, sp, num);
		break;
	  }
	case 's':
		FmtWriteString(ctx, sp, va_arg(ap, const char*));
		break;
	case '{':
	case '}': {
		sp->front = ch == '{' ? 0 : 1;

		ch = FmtGet(ctx);
		sp->padch = (ch == 'c' ? va_arg(ap, uint32_t) : ch);

		ch = FmtGet(ctx);

		if(ch >= '0' && ch <= '9') {
			while(ch >= '0' && ch <= '9') {
				sp->pad = sp->pad * 10 + (ch - '0');
				ch = FmtGet(ctx);
			}
		} else {
			sp->pad = FormatType(ch) == 3 ? va_arg(ap, uint64_t) : va_arg(ap, uint32_t);
			FmtGet(ctx);
		}


		FormatExpr(ctx, sp, ap);

		break;
	  }
	}

	*sp = (struct FormatSpec) { 0, 0, 0, 0 };
}

size_t Format(char *str, size_t size, const char *fmt, va_list ap)
{
	struct FormatCtx ctx = (struct FormatCtx)  { str, size, fmt, 0 };
	struct FormatSpec sp = (struct FormatSpec) { 0, 0, 0, 0 };


	while(*ctx.fmt) {
		char ch = FmtGet(&ctx);

		if(ch != '%') {
			FmtPut(&ctx, ch);
			continue;
		}

		FormatExpr(&ctx, &sp, ap);
	}

	return ctx.spos;
}
