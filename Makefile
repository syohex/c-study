all:
	echo support only "make format"

.PHONY: format
format:
	git ls-files | grep -E '\.(c|cpp|cc|h)$$' | xargs clang-format -i
