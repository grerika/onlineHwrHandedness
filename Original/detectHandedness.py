#!/usr/bin/env python

## ID
UNDEC   = 0
LEFT    = 1
RIGHT   = 2
RVERT   = 10
LVERT   = 20
UVERT   = 30
DELETED = 99
NC = 12
## BOOLEAN PARAMETERS
DEBUG1 = 0
DEBUG2 = 0
STAT   = 0
PLOT_LINES = 0
PLOT_ORIG  = 0
PLOT_ANNOTATED  = 1
VERTICAL  = 0
## OTHER PARAMETERS
DEV_LIMIT   = 0.4
HORIZ_LIMIT = 0.4
VERTI_LIMIT = 1.0
ANGLE_LIMIT = 0.001
LTHR   = 1

IMGDIR = "images"
import xml.etree.ElementTree as etree
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import math
from sklearn import linear_model, datasets
from pylab import *
from pluss import *
from types import *

############################################################################
################################### MAIN ###################################
############################################################################
line="---------------------------------------------------------------------------------------"
if DEBUG2:
	print "Parameters: "
	for arg in sys.argv: print "\t" + arg

## First parameter is the writing sample
if len(sys.argv) > 1 :
	XmlFilename = sys.argv[1]
	if len(sys.argv) > 4:
		HORIZ_LIMIT = float(sys.argv[2])
		DEV_LIMIT    = float(sys.argv[3])
else : #default sample
	XmlFilename = "Data/a01/a01-001/strokesz.xml"

if PLOT_ORIG or PLOT_LINES or PLOT_ANNOTATED:
	if not os.path.exists(IMGDIR): os.makedirs(IMGDIR)
	cmap = get_cmap(NC)
##################################################################
## READ XML file
##################################################################
writerID   = getWriterID(XmlFilename)
handedness = getWriterHandedness(writerID)
(x_stroke_orig,y_stroke_orig) = getAllStrokes(XmlFilename)
if DEBUG2: print "writerID:", writerID, "Handedness: ", handednessStr(handedness)
##################################################################
## STATISTIC							##
##################################################################
if STAT: printStr = "before noise removal"
else: printStr = ""
width, height, diagonal, length, avg_length, dev_length, avg_height, avg_width, avg_diagonal,avg_distance, overallminx, overallmaxx, overallminy,overallmaxy,sum1length,cnt = statistics(x_stroke_orig,y_stroke_orig,printStr)
if sum1length == 0:
	print "No point found, exit..."
	exit(-1)

#################################################################
## Iterating over the strokes again				##
##################################################################
deleted = removeNoise(x_stroke_orig,y_stroke_orig,avg_width,avg_height)
cnt -= len(deleted)
##################################################################
## STATISTIC after denoising					##
##################################################################
if len(deleted)==0:
	if DEBUG1: print "Noise was not found." 
elif DEBUG1:
	print "\tNo  points removed: %d" % (len(deleted))		

if STAT: printStr = "after noise removal"
else: printStr = ""
width, height, diagonal, length, avg_length, dev_length, avg_height, avg_width, avg_diagonal,avg_distance, overallminx, overallmaxx, overallminy,overallmaxy,sum1length,cnt = statistics(x_stroke_orig,y_stroke_orig,printStr)

##################################################################
## LINE DETECTION, WORD DETECTION				##
##################################################################
(lineid,x_lines,y_lines,cx,cy,lx,ly,ux,uy) = lineDetection(x_stroke_orig,y_stroke_orig,overallmaxx,overallminx,avg_length,DEBUG1,DEBUG2)

(lineAngleList,lineSlopeList,lineInterceptList,lineLowerSlopeList,lineLowerInterceptList,lineUpperSlopeList,lineUpperInterceptList) = angleDetection(cx,cy,lx,ly,ux,uy,overallminx,overallmaxx,DEBUG2)
##################################################################
## STATISTICS FOR LINE AND SLOPE OF LINES			##
##################################################################
(lineAvgAngle,lineAvgHeight) = statisticsLine(lineInterceptList,lineAngleList)
##################################################################
## CORRECTION SKEW (ROTATION)					##
##################################################################
(x_lines_orig,y_lines_orig,x_all_stroke,y_all_stroke) = skewCorrection(x_lines,y_lines,x_stroke_orig,y_stroke_orig,lineAvgAngle,ANGLE_LIMIT)
if STAT: printStr = "after rotation"
else: printStr = ""
width, height, diagonal, length, avg_length, dev_length, avg_height, avg_width, avg_diagonal,avg_distance, overallminx, overallmaxx, overallminy,overallmaxy,sum1length,cnt = statistics(x_all_stroke,y_all_stroke,printStr)

if STAT:
	print line
	print "\tdecision\tx start/end [length]\t\tslope (max/dev/avg)"
##################################################################
## HORIZONTAL AND VERTICAL STROKES
##################################################################
(stroke_horiz_LtoR,stroke_horiz_RtoL,cnt_short,cnt_short_horiz,total_right,total_left) = horizontalStrokeDetection(x_all_stroke,y_all_stroke,x_stroke_orig,y_stroke_orig,length,diagonal,avg_diagonal,
lineid,lineLowerSlopeList,lineLowerInterceptList,lineUpperSlopeList,lineUpperInterceptList,
DEBUG1,DEBUG2,STAT,DEV_LIMIT, HORIZ_LIMIT)


# VERTICAL STROKE
if VERTICAL:
	if STAT:
		print line + "\n\tdecision\tx start/end [length]\t\tslope"
	(stroke_vert_right,stroke_vert_left,stroke_vert_undec,cnt_long, cnt_vertical) = verticalStrokeDetection(x_all_stroke,y_all_stroke,length,height,avg_height,STAT)
	
## END OF ANALYSIS
##################################################################
## STATISTIC comes... (horizontal, vertical strokes)		##
##################################################################
right_handed = len(stroke_horiz_LtoR)
left_handed  = len(stroke_horiz_RtoL)
if VERTICAL:
	rvert_cnt    = len(stroke_vert_right)
	lvert_cnt    = len(stroke_vert_left)
	uvert_cnt    = len(stroke_vert_undec)
if STAT:
	print "------------------------- Overall stroke statistics -----------------------------------"
	print "No. of lines: %d\n\tAverage angle: %.2f\theight: %5d" % (len(lineAngleList),lineAvgAngle,lineAvgHeight)
	print "No strokes: %3d\n\tShort strokes\t%5d (%2.2f%%)\n\tShort&horiz\t%5d (%2.2f%%)" % (cnt,cnt_short,cnt_short*100/cnt,cnt_short_horiz,cnt_short_horiz*100/cnt)
	if VERTICAL and cnt_long>0: print "\tLong strokes\t%5d (%2.2f%%)" % (cnt_long,cnt_long*100/cnt)	
	if cnt_short_horiz>0:
		print "No of horizontal strokes:"
		print "\t\tleft\tright\n\t\t%d\t%d\n\t\t%2.2f%%\t%2.2f%%" % (left_handed,right_handed,left_handed*100/cnt_short_horiz,right_handed*100/cnt_short_horiz)
	else:
		print "\t\tNo short and horizontal stroke"
	if VERTICAL and cnt_vertical + cnt_long > 0:
		print "No of vertical strokes: %3d" % (cnt_vertical)
		if cnt_vertical >0:
			print "\t\tright/\tleft\\\tundec|\n\t\t%d\t%d\t%d\n\t\t%2.2f%%\t%2.2f%%\t%2.2f%%" % (rvert_cnt,lvert_cnt,uvert_cnt,rvert_cnt*100/cnt_vertical,lvert_cnt*100/cnt_vertical,uvert_cnt*100/cnt_vertical)

	print "------------------------------- Final decision ----------------------------------------"
##################################################################
## DECISION							##
##################################################################
final_text = "\t" + XmlFilename
plotTitle = XmlFilename.replace('.xml','').split("/")
if len(plotTitle) >=3:
	sampleID = plotTitle[-3] + "_" + plotTitle[-2] + "_" + plotTitle[-1]
	plotTitle = "F [" + plotTitle[-3]+"/" + plotTitle[-2] + "/" + plotTitle[-1] + "] W [" + writerID + "] H [" + handednessStr(handedness)[0].upper()  + "]"
else:
	sampleID  = XmlFilename.replace('.xml','')
	plotTitle = sampleID

plotBaseName = plotTitle
if cnt_short_horiz <1:
	final_decision = UNDEC
	plotTitle += " D [U]"
elif right_handed > left_handed and left_handed == 0:
	final_decision = RIGHT
	plotTitle +=  " D [R]"
elif left_handed > right_handed or left_handed > 0:
	final_decision = LEFT
	plotTitle +=  " D [L]"
else:
	if (rvert_cnt > 0 and lvert_cnt == 0):
		final_decision = RIGHT
		plotTitle +=  " D [Rv]"				
	elif (lvert_cnt > 0):
		final_decision = LEFT
		plotTitle +=  " D [Lv]"
	else:
		final_decision = UNDEC
		plotTitle +=  " D [?]"
if DEBUG2: print handednessStr(final_decision) + final_text
##################################################################
## RESULT							##
##################################################################
if handedness == RIGHT:
	if final_decision == handedness and left_handed == 0:
		final_text =  "1 0 0 0 0 0"
	elif final_decision == handedness and left_handed > 0:
		final_text =  "! 0 0 0 0 0"
	elif final_decision == LEFT and right_handed == 0:
		final_text =  "0 0 1 0 0 0"
	elif final_decision == LEFT and right_handed > 0:
		final_text =  "0 0 ! 0 0 0"
	else:
		final_text =  "0 0 0 0 1 0"
elif handedness == LEFT:
	if final_decision == handedness and right_handed == 0:
		final_text =  "0 1 0 0 0 0"
	elif final_decision == handedness and right_handed > 0:
		final_text =  "0 ! 0 0 0 0"
	elif final_decision == RIGHT and left_handed == 0:
		final_text =  "0 0 0 1 0 0"
	elif final_decision == RIGHT and left_handed > 0:
		final_text =  "0 0 0 ! 0 0"
	elif final_decision == UNDEC:
		final_text =  "0 0 0 0 0 1"
else: # handedness == UNDEC
	final_text =  "0 0 0 0 0 0"
final_text += " "  + sampleID.replace("_", "/") + ".xml" + " " + writerID
if DEBUG1: print final_text

if VERTICAL:
	sys.stderr.write(sampleID + "," +writerID + "," + handednessStr(handedness)+","+handednessStr(final_decision)+","+str(left_handed) + "," +str(right_handed)+","+str(rvert_cnt)+","+str(lvert_cnt)+"\n")
else:
	sys.stderr.write(sampleID + "," +writerID + "," + handednessStr(handedness)+","+handednessStr(final_decision)+","+str(left_handed) + "," +str(right_handed)+"\n")

##################################################################
### PLOTTING							##
##################################################################
if PLOT_ORIG:
	fileNameOrigBase = IMGDIR + "/" + writerID + "_" + sampleID[4:11].upper() + "_" + sampleID[-1].upper() + "_" + handednessStr(handedness)[0].upper()
	# DENOISED WITH LINES
	fileName = fileNameOrigBase + "_denoised_orig_strokes_w_lines.pdf"
	plotTitle = "%s A [%.2f] Height [%d]" % (plotBaseName, lineAvgAngle,lineAvgHeight)	
	plotStrokes(x_stroke_orig,y_stroke_orig,fileName,plotTitle,"denoised-strokes")
	if DEBUG1: sys.stdout.write("\tOutput: " + fileName + "\n")
	
	#DENOISED WITHOUT LINES
	fileName = fileNameOrigBase + "_denoised_orig_strokes_wo_lines.pdf"
	plotStrokes(x_stroke_orig,y_stroke_orig,fileName,plotTitle,"denoised-strokes")	
	if DEBUG1: sys.stdout.write("\tOutput: " + fileName + "\n")

	# ORIGINAL STROKE
	plotFilename  = fileNameOrigBase + "_orig_strokes.pdf"
	plotStrokes(x_stroke_orig,y_stroke_orig,plotFilename,plotTitle,"original-strokes")
	if DEBUG1: sys.stdout.write("\tOutput: " + plotFilename + "\n")
	
if VERTICAL: label = getLabels(cnt,stroke_vert_right,stroke_vert_left,stroke_vert_undec,stroke_horiz_LtoR,stroke_horiz_RtoL)
else:  label = getLabels(cnt,[],[],[],stroke_horiz_LtoR,stroke_horiz_RtoL)
if PLOT_LINES:
	fileName = fileNameOrigBase +"_lines.pdf"
	plotTitle =  "%s - %d lines, %d height" % (plotBaseName, len(lineAngleList),lineAvgHeight) 
	plotStrokes(x_lines_orig,y_lines_orig,fileName,plotTitle,"lines",lineAngleList,x_lines,y_lines)
	if DEBUG1: sys.stdout.write("\tOutput: " + fileName + "\n")

if PLOT_ANNOTATED:
	sample = writerID + "_" + sampleID[4:11].upper() + "_" + sampleID[-1].upper()

	plotTitle = "%s R [%d] L [%d]" % (plotBaseName, right_handed,left_handed)
	if VERTICAL: plotTitle += " LV[" + str(lvert_cnt) + "] RV[" + str(rvert_cnt) + "]"			
		
	fileBase = IMGDIR +  "/D" + str(DEV_LIMIT) + "_H" + str(HORIZ_LIMIT) + "_" + sample + "_" + handednessStr(final_decision)[0].upper() + "_" + handednessStr(handedness)[0].upper() 


	fileName = fileBase +"_annotated.pdf"
	plotStrokes(x_all_stroke,y_all_stroke,fileName,plotTitle,"annotated",label)
	if DEBUG1: sys.stdout.write("\tOutput: " + fileName + "\n")


	fileName = fileBase  + "_denoised_annotated.pdf"
	plotStrokes(x_all_stroke,y_all_stroke,fileName,plotTitle,"denoised-annotated",label,[],[],deleted)
	if DEBUG1: sys.stdout.write("\tOutput: " + fileName + "\n")

quit()
