
struct base {
	int a;
	int b;
	char c;
};

struct base_with_ptr {
	struct base d;
	char *e;
};

struct mixed {
	struct base f;
	struct base_with_ptr g;
	struct base *h;
	int i, j;
	char *k;
};

int main()
{
	struct mixed m;
	return 1;
}
