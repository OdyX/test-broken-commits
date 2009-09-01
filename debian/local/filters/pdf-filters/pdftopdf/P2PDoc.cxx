/*

Copyright (c) 2006-2007, BBR Inc.  All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
/*
 P2PDoc.cc
 pdftopdf doc
*/

#include <config.h>
#include "goo/gmem.h"
#include "P2PDoc.h"
#include "P2PXRef.h"
#include "XRef.h"
#include "P2PCatalog.h"
#include <time.h>

P2PDoc::Options P2PDoc::options;

/* Constructor */
P2PDoc::P2PDoc(PDFDoc *orgDocA)
{
  orgDoc = orgDocA;
  catalog = new P2PCatalog(orgDoc->getCatalog(),orgDoc->getXRef());
}

P2PDoc::~P2PDoc()
{
  delete catalog;
  P2PXRef::clean();
}

void P2PDoc::output(P2POutputStream *str, int deviceCopies, bool deviceCollate)
{
  int xrefOffset;
  int num, gen;
  time_t        curtime;
  struct tm     *curtm;
  char          curdate[255];
  char	version[10];
  XRef *xref = orgDoc->getXRef();
  Object obj;
  GBool outputTitle = gFalse;
  GBool outputCreationDate = gFalse;
  GBool outputModDate = gFalse;
  GBool outputTrapped = gFalse;
  GBool outputProducer = gFalse;

  /* get time data */
  curtime = time(NULL);
  curtm = localtime(&curtime);
  strftime(curdate, sizeof(curdate),"D:%Y%m%d%H%M%S%z", curtm);

  /* output header */
  snprintf(version,sizeof(version),"%%PDF-%d.%d\n",
     orgDoc->getPDFMajorVersion(),orgDoc->getPDFMinorVersion());
  str->puts(version);
  str->puts("%\0201\0202\0203\0204\n");
  str->puts("% This file was generated by pdftopdf\n");

  /*
   * output device copies commen
   * This is not standard, but like PostScript.
   */
  if (deviceCopies > 0) {
    str->printf("%%%%PDFTOPDFNumCopies : %d\n",deviceCopies);
    str->printf("%%%%PDFTOPDFCollate : %s\n",deviceCollate ? "true" : "false");
  }

  /* output body */
  catalog->output(str,options.copies,options.collate);
  /* output objects that are not output yet. */
  P2PXRef::flush(str,xref);

  /* output cross reference */
  xrefOffset = P2PXRef::output(str);

  /* output trailer */
  catalog->getNum(&num,&gen);
  str->puts("trailer\n");
  str->printf("<< /Size %d /Root %d %d R \n",P2PXRef::getNObjects(),
    num,gen);

  str->puts("/Info << ");
  if (orgDoc->getDocInfo(&obj) == 0) {
    Dict *info = obj.getDict();
    int i;
    int n = info->getLength();

    for (i = 0;i < n;i++) {
#ifdef HAVE_UGOOSTRING_H
      char *key = info->getKey(i)->getCString();
#else
      char *key = info->getKey(i);
#endif
      Object val;

      if (info->getValNF(i,&val) != 0) {
	P2POutput::outputName(key,str);
	str->putchar(' ');
	P2POutput::outputObject(&val,str,xref);
	if (strcmp(key,"Producer") == 0) {
	  outputProducer = gTrue;
	} else if (strcmp(key,"Trapped") == 0) {
	  outputTrapped = gTrue;
	} else if (strcmp(key,"ModDate") == 0) {
	  outputModDate = gTrue;
	} else if (strcmp(key,"CreationDate") == 0) {
	  outputCreationDate = gTrue;
	} else if (strcmp(key,"Title") == 0) {
	  outputTitle = gTrue;
	}
      }

      val.free();
#ifdef HAVE_UGOOSTRING_H
      delete[] key;
#endif
    }
    obj.free();
  }
  if (!outputTitle && options.title != 0) {
    str->puts("/Title ");
    P2POutput::outputString(options.title,strlen(options.title),str);
    str->putchar(' ');
  }
  if (!outputCreationDate) str->printf("/CreationDate (%s) ",curdate);
  if (!outputModDate) str->printf("/ModDate (%s) ",curdate);
  if (!outputProducer) str->puts("/Producer (pdftopdf) ");
  if (!outputTrapped) str->puts("/Trapped /False");
  str->puts(" >>\n");

  str->puts(">>\n");
  str->puts("startxref\n");
  str->printf("%d\n",xrefOffset);
  str->puts("%%EOF\n");
}

int P2PDoc::nup(int n, PDFRectangle *box,
  unsigned int borderFlag, unsigned int layout,
  int xpos, int ypos)
{
  if (n == 1) return 0;
  switch (n) {
  case 2:
  case 4:
  case 6:
  case 8:
  case 9:
  case 16:
    break;
  default:
    return -1;
  }
  return catalog->nup(n,box,borderFlag,layout,xpos,ypos);
}

void P2PDoc::select()
{
  catalog->select(options.pageSet,options.pageRanges);
}

void P2PDoc::fit(PDFRectangle *box, double zoom)
{
  catalog->fit(box,zoom);
}

void P2PDoc::mirror()
{
  catalog->mirror();
}

void P2PDoc::rotate(int orientation)
{
  catalog->rotate(orientation);
}

void P2PDoc::position(PDFRectangle *box, int xpos, int ypos)
{
  catalog->position(box,xpos,ypos);
}

void P2PDoc::scale(double zoom)
{
  catalog->scale(zoom);
}

void P2PDoc::autoRotate(PDFRectangle *box)
{
  catalog->autoRotate(box);
}

void P2PDoc::setMediaBox(PDFRectangle *mediaBoxA)
{
  catalog->setMediaBox(mediaBoxA);
}

int P2PDoc::getNumberOfPages()
{
  return catalog->getNumberOfPages();
}
