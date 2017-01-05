# Makefile for documentation

.PHONY: all clean

MD_SOURCE != find md -type f -name '*.md'

HTML_OUT   = $(MD_SOURCE:.md=.html)

MARKDOWN = pandoc --from markdown_github --to html --standalone

all: html

html: $(HTML_OUT)
	mv `find md -type f -name '*.html'` html/

clean:
	rm -f html/*
	rm -rf *.bak *~

%.html: %.md
	$(MARKDOWN) $< --output $@
