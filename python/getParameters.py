#!/usr/bin/env python
import os,sys
import PyPDF2

for version in range(0,100):
  if os.path.isfile('v'+ str(version) + '/Output/all.pdf'):
    pdf_file = open('v'+ str(version) + '/Output/all.pdf')
    read_pdf = PyPDF2.PdfFileReader(pdf_file)
    number_of_pages = read_pdf.getNumPages()
    page = read_pdf.getPage(2)
    page_content = page.extractText()
    parameters = []
    parameters.append(float(page_content.split('\n')[1].split('.')[0] + "." + page_content.split('\n')[1].split('.')[1][0:2]))
    parameters.append(float(page_content.split('\n')[1].split('.')[1][2:len(page_content.split('\n')[1].split('.')[1])] + "." + page_content.split('\n')[1].split('.')[2][0:2]))
    if "c2" in page_content.split('\n')[1]:
      parameters.append(float(page_content.split('\n')[1].split('c1')[1].split('c2')[0]))
    else:
      parameters.append(float(page_content.split('\n')[1].split('c1')[1].split('B')[0]))
      sys.exit(0)
    if "c3" in page_content.split('\n')[1]:
      parameters.append(float(page_content.split('\n')[1].split('c2')[1].split('c3')[0]))
      parameters.append(float(page_content.split('\n')[1].split('c3')[1].split('B')[0]))
    else:
      parameters.append(float(page_content.split('\n')[1].split('c2')[1].split('B')[0]))
    print parameters
