
import math
import xml.etree.ElementTree as etree
import matplotlib.pyplot as plt
import matplotlib.cm as cmx
import matplotlib.colors as colors
from scipy import stats

writerFilename= "writers.xml"
UNDEC = 0
LEFT   = 1
RIGHT = 2
RVERT = 10
LVERT  = 20
UVERT  = 30
DELETED = 99
NC = 12

BOOL_MIDDLE_ZONE = 1

def handednessStr(n):
	if n == UNDEC:
		return "undec"
	elif n == LEFT:
		return "left"
	elif n == RIGHT:
		return "right"
	elif n == RVERT:
		return "rvert"
	elif n == LVERT:
		return "lvert"
	elif n == UVERT:
		return "uvert"

	else:
		return "n/a"


def getWriterHandedness(writerID,writerFilename="writers.xml"):
	tree = etree.parse(writerFilename)
	root = tree.getroot()
	ret = UNDEC
	for child in root:
		if child.tag == "Writer" :
			tmp = "0"
			if child.get("name") == writerID:
				tmp = child.get("WritingType")
				if tmp == "Left-handed":
					ret = LEFT
					return ret
				elif tmp == "Right-handed":
					ret = RIGHT
					return ret
	return ret
	

						
def getWriterID(XmlFilename,writerFilename="writers.xml"):
	tree = etree.parse(XmlFilename)
	root = tree.getroot()
	writerID=""
	for child in root:
		if child.tag == "General" :
			for child2 in child:
				if child2.tag == "Form":
					writerID = child2.get('writerID',default = 0)
	return writerID


def getAllStrokes(XmlFilename):
	x = []
	y = []
	tree = etree.parse(XmlFilename)
	root = tree.getroot()
	for child in root:
		if child.tag == "StrokeSet":
			if getWriterID(XmlFilename) == "":
				print "writerID not found, exit..."
				return x
			for Stroke in child:
				x.append([])
				y.append([])			
				for points in Stroke:
					xtmp = int(points.get("x"))
					ytmp = int(points.get("y"))
					if xtmp is not None and ytmp is not None and xtmp > 0 and ytmp > 0:
						x[-1].append(int(xtmp))
						y[-1].append(-int(ytmp))
	return (x, y)

def lengthList(x):
	length = []
	for it in range(len(x)):
		length.append(len(x[it]))
	return length
	
def removeNoise(x,y,avg_width,avg_height):
	cnt = len(x)
	deleted = []
	
	length = lengthList(x)
	for k in range(0,2):
		it = 0
		while it < cnt:
			if length[it]>1:
				point_it=1
				while point_it < length[it]:
					distx = abs(x[it][point_it-1]-x[it][point_it])
					disty = abs(y[it][point_it-1]-y[it][point_it])
					isFar = distx+disty > 2*(avg_width + avg_height)
					if isFar:
						length[it] -= 1
						xtmp = x[it].pop(point_it)
						ytmp = y[it].pop(point_it)
						deleted.append([xtmp,ytmp])
					point_it += 1
				it += 1
			else:
#				print "Deleted stroke"
				cnt -= 1
				x.pop(it)
				y.pop(it)
		if len(deleted)==0: break
	return (deleted)


def lineDetection(x,y,overallmaxx,overallminx,avg_length,DEBUG1=False,DEBUG2=False):
	x_lines = []
	y_lines = []
	cx = []
	cy = []
	ux = []
	uy = []
	lx = []
	ly = []
	centerlistx = []
	centerlisty = []
	lowerlistx  = []
	lowerlisty  = []
	upperlistx  = []
	upperlisty  = []


	cnt = len(x)
	lineid =  [0] * cnt
	
	length = lengthList(x)
	new_line = 0
	for it in range(cnt-1):
		if length[it-1]*length[it]>0 and abs(max(x[it-1])-min(x[it]))> 0.5*(overallmaxx-overallminx):
			new_line = 1
			x_lines.append([])
			y_lines.append([])
			cx.append([])
			cy.append([])
			lx.append([])
			ly.append([])			
			ux.append([])
			uy.append([])
		else:
			if len(x_lines)==0:
				x_lines.append([])
				y_lines.append([])
		x_lines[-1].extend(x[it])
		y_lines[-1].extend(y[it])
		lineid[it] = len(x_lines)-1
	
		# center point
		if length[it] > avg_length*0.5:
			xtmp,ytmp = centerlist(x[it],y[it])
			if (new_line):
		       		new_line = 0		       	
			       			
			       	cx[-1].extend(centerlistx)
			       	cy[-1].extend(centerlisty)
		       	 	ux[-1].extend(upperlistx)
			       	uy[-1].extend(upperlisty)
		       	 	lx[-1].extend(lowerlistx)
			       	ly[-1].extend(lowerlisty)

		       		centerlistx = []
		       		centerlisty = []
		       		lowerlistx = []
		       		lowerlisty = []
		       		upperlistx = []
		       		upperlisty = []

			centerlistx.append(xtmp)
       			centerlisty.append(ytmp)
	
	  		lowerx = x[it][argmin(y[it])]
	      		lowery = min(y[it])
	       		lowerlistx.append(lowerx)
		       	lowerlisty.append(lowery)
	
			upperx = x[it][argmax(y[it])]
		       	uppery = max(y[it])
		       	upperlistx.append(upperx)
		       	upperlisty.append(uppery)

			tmp = plt.plot(xtmp, ytmp)
		       	plt.setp(tmp, markersize=3.0,marker="s",color="0.5",markeredgecolor="0.5")
			tmp = plt.plot(lowerx,lowery)
		       	plt.setp(tmp, markersize=3.0,marker="v",color="0.3",markeredgecolor="0.3")
			tmp = plt.plot(upperx,uppery)
		       	plt.setp(tmp, markersize=3.0,marker="^",color="0.3",markeredgecolor="0.3")
	##################################################################
	## LAST line - slope, intercept, angle				##
	##################################################################
	if DEBUG2: print "Centerx, centery: ", cx, cy
	lineid[cnt-1] = len(x_lines)-1		


	return (lineid,x_lines,y_lines,cx,cy,lx,ly,ux,uy)

def angleDetection(cx,cy,lx,ly,ux,uy,overallminx,overallmaxx,DEBUG=False):
	lineAngleList = []
	lineInterceptList = []
	lineSlopeList = []
	lineUpperInterceptList = []
	lineUpperSlopeList = []
	lineLowerInterceptList = []
	lineLowerSlopeList = []	
	# Fit line on the last line if enough point is available
	for it in range(len(cx)):
		if len(cx[it])>0:
			lineslope, intercept, r_value, p_value, std_err = stats.linregress(cx[it],cy[it])
			plotLinearRegression(plt,overallminx,overallmaxx,lineslope, intercept,"0.5")
			lineAngle = math.atan(lineslope)
			lineAngleList.append(lineAngle)
			lineSlopeList.append(lineslope)						
			lineInterceptList.append(intercept)
			if DEBUG:
				print "Slope: %.2f, Intercept: %.2f, Angle: %.2f" % (lineslope,intercept,lineAngle)
				print "Centerx, centery: ", cx, cy
			lineslope, intercept, r_value, p_value, std_err = stats.linregress(lx[it],ly[it])
			plotLinearRegression(plt,overallminx,overallmaxx,lineslope, intercept,"0.3")					
			lineLowerSlopeList.append(lineslope)						
			lineLowerInterceptList.append(intercept)

			lineslope, intercept, r_value, p_value, std_err  = stats.linregress(ux[it],uy[it])
			plotLinearRegression(plt,overallminx,overallmaxx,lineslope, intercept,"0.3")
			lineUpperSlopeList.append(lineslope)						
			lineUpperInterceptList.append(intercept)
	
	return (lineAngleList,lineSlopeList,lineInterceptList,lineLowerSlopeList,lineLowerInterceptList,lineUpperSlopeList,lineUpperInterceptList)

def verticalStrokeDetection(x,y,length,height,avg_height,STAT=False):
	cnt_long = 0
	cnt_vertical = 0
	stroke_vert_right  = []
	stroke_vert_left   = []	
	stroke_vert_undec  = []
	
	all_avg1_x = 0
	all_avg2_x = 0		
	cnt = len(x)
	for it in range(cnt):
		all_avg1_x  += sum(x[it])
		for it2 in range(len(x[it])):
			all_avg2_x  += pow(x[it][it2],2)
	all_avg1_x /= length[it]
	all_avg2_x /= length[it]
	
	all_dev_x = all_avg2_x-all_avg1_x*all_avg1_x
	if all_dev_x > 0: 
		all_dev_x  = math.sqrt(all_dev_x)
	elif abs(all_dev_x) < 0.0000001:  # too small
		all_dev_x = 0
	else: 
		all_dev_x = 100	

	for it in range(cnt):
		if height[it]>avg_height:
			cnt_long += 1
			avg1_x  = sum(x[it])/length[it]
			avg2_x  = 0
			for it2 in range(len(x[it])):
				avg2_x += pow(x[it][it2],2)
			tmp = avg2_x/length[it]-avg1_x*avg1_x
			if tmp > 0: 
				dev_x  = math.sqrt(tmp)
			elif abs(tmp) < 0.0000001:  # too small
				dev_x = 0
			else: 
				dev_x = 100
			if abs(dev_x) < all_dev_x*0.1*height[it]/avg_height:
				xdiff = x[it][0]-x[it][-1]
				cnt_vertical += 1					
				if (abs(xdiff)>0.0000001):
					slopevert = (y[it][0]-y[it][-1])/xdiff
					if slopevert > 6 and slopevert <12:										
						act_decision = RVERT
						stroke_vert_right.append(it)
					elif slopevert > -12 and slopevert <-6:
						act_decision = LVERT
						stroke_vert_left.append(it)
					else:
						act_decision = UVERT
						stroke_vert_undec.append(it)
				else:
					slopevert    = 100000
					act_decision = UVERT
					stroke_vert_undec.append(it)
					print "Too large slope"
				if STAT :
					print "%16s\t%6d %6d [%d]\t\t%5.2f" % (handednessStr(act_decision),x[it][0], x[it][length[it]-1],length[it],slopevert)
					
	return (stroke_vert_right,stroke_vert_left,stroke_vert_undec,cnt_long, cnt_vertical) 
	
def horizontalStrokeDetection(x,y,x_orig,y_orig,length,diagonal,avg_diagonal,lineid,lsl,lil,usl,uil,DEBUG1=False,DEBUG2=False,STAT=False,DEV_LIMIT=0.4, HORIZ_LIMIT=0.4):
	stroke_horiz_RtoL = []
	stroke_horiz_LtoR  = []	

	cnt_short_horiz = cnt_short = 0
	total_left = total_right = 0
	cnt = len(x)
	for it in range(cnt-1):
		# Right/Left strokes
		actual_right  = actual_left = 0
		for i in range(0,length[it]-2):
			if x[it][i] < x[it][i+1]:# --> from left to right
				actual_right += 1
			else:
				actual_left  += 1# <--  from right to left
		total_right += actual_right
		total_left  += actual_left
		if actual_right > 2*actual_left:  # absolute right
			col = "c"
		elif actual_left > 2*actual_right: # absolute left
			col = "m"
		else: # undecided
			col = "y"
		# HORIZONTAL STROKE
		slope_cnt = sum1slope = sum2slope = dev_slope = 0;
		max_slope = -100		
		for k in range(1,length[it]-2):
			if x[it][k+1]-x[it][k] != 0:
				slope = float(y[it][k+1]-y[it][k])/(x[it][k+1]-x[it][k])
				sum1slope += slope
				sum2slope += pow(slope,2)
				slope_cnt += 1
				if max_slope < abs(slope):
					max_slope = abs(slope)
					if slope > 0:
						sgn_slope = +1
					else:
						sgn_slope = -1
		if slope_cnt != 0:
			avg1_slope = sum1slope/float(slope_cnt)
			avg2_slope = sum2slope/float(slope_cnt)
			tmp = avg2_slope-math.pow(avg1_slope,2)
			if tmp >0:
				dev_slope  = math.sqrt(tmp)
			elif abs(tmp) < 0.0000001:
				dev_slope = 0
			else:
				dev_slope = 100
		else: 
			if DEBUG2: print "\tNo valid slope found!"
			avg1_slope = 0;
			dev_slope = 100;

		# in the middle zone	
		if lineid[it] < len(usl) and BOOL_MIDDLE_ZONE:
			in_middle_zone = list_between_lines(x_orig[it],y_orig[it],usl[lineid[it]],uil[lineid[it]],lsl[lineid[it]],lil[lineid[it]])
		else:
			in_middle_zone = 1
		
		if diagonal[it] > 0.25*avg_diagonal and in_middle_zone:
			cnt_short += 1
			if DEBUG2 : print "SHORT, size=",length[it]
			if dev_slope < DEV_LIMIT and abs(avg1_slope) < HORIZ_LIMIT:
				cnt_short_horiz += 1
				if x[it][0] < x[it][-1]:
					act_decision = RIGHT
					stroke_horiz_LtoR.append(it)
				else:
					act_decision = LEFT
					stroke_horiz_RtoL.append(it)						
				if STAT :
					print "%16s\t%6d %6d [%d]\t\t%.4f  %.4f %.4f" % (handednessStr(act_decision),x[it][0], x[it][length[it]-1],length[it],max_slope, dev_slope,avg1_slope)
				if DEBUG2: print "\thorizontal (max slope=",max_slope," dev slope=",dev_slope," avg slope=",avg1_slope,")"
			else:
				if DEBUG2: print "\tnot horizontal (max slope=",max_slope," dev slope=",dev_slope," avg slope=",avg1_slope,")"

	return (stroke_horiz_LtoR,stroke_horiz_RtoL,cnt_short,cnt_short_horiz,total_right,total_left)


def statisticsLine(lineInterceptList,lineAngleList):
	heightLine = []
	for i in range(1,len(lineInterceptList)):
		heightLine.append(abs(lineInterceptList[i]-lineInterceptList[i-1]))
	if len(lineAngleList)>0:
		lineAvgAngle  = sum(lineAngleList)/len(lineAngleList)
		lineAvgHeight = sum(heightLine)/len(heightLine)
	else:
		lineAvgAngle  = 0.0
		lineAvgHeight = 0.0
	return (lineAvgAngle,lineAvgHeight)
	
def inside(c,a,b):
	if (c>a and c<b) or (c>b and c<a):
		return 1
	else:
		return 0
		
def crosss(x1,y1,x2,y2):
	if len(x1)*len(y1)*len(x2)*len(y2)==0:
		return 0
		
	PAD=7
	x1min=min(x1)-PAD
	x2min=min(x2)-PAD
	y1min=min(y1)-PAD
	y2min=min(y2)-PAD
	x1max=max(x1)+PAD
	x2max=max(x2)+PAD
	y1max=max(y1)+PAD
	y2max=max(y2)+PAD
	
	if (inside(x1min,x2min,x2max) and inside(x1max,x2min,x2max)) or (inside(y1min,y2min,y2max) and inside(y1min,y2min,y2max)):
		return 1
	else:
		return 0
		
		
def plotLinearRegression(plt,overallminx,overallmaxx,slope, intercept,col):
	listX = range(overallminx,overallmaxx)
	listY=[]
	for i in range(0,len(listX)):
		listY.append(listX[i]*slope+intercept)
		
#	print listX,listY
	tmp = plt.plot(listX,listY)
        plt.setp(tmp, linewidth=1.0,color=col)
        
        return 0
        
        
from math import cos, sin

def skewCorrection(x_lines,y_lines,x_stroke_orig,y_stroke_orig,lineAvgAngle,ANGLE_LIMIT):
	x_lines_orig = list(x_lines)
	y_lines_orig = list(y_lines)
	x_all_stroke = list(x_stroke_orig)
	y_all_stroke = list(y_stroke_orig)
	if abs(lineAvgAngle)>ANGLE_LIMIT:
		for it in range(len(x_all_stroke)):
			(x_all_stroke[it], y_all_stroke[it]) = make_rotation_transformation(-lineAvgAngle,x_all_stroke[it],y_all_stroke[it])
		for it in range(len(x_lines)):
			(x_lines[it], y_lines[it]) = make_rotation_transformation(-lineAvgAngle,x_lines[it],y_lines[it])
	return (x_lines_orig,y_lines_orig,x_all_stroke,y_all_stroke)
		
def make_rotation_transformation(angle,x,y):
	cos_theta, sin_theta = cos(angle), sin(angle)
	xtmp = []
	ytmp = []
	for i in range(0,len(x)):
		xtmp.append( x[i] * cos_theta - y[i] * sin_theta )
		ytmp.append( x[i] * sin_theta + y[i] * cos_theta )
	return (xtmp, ytmp)
	
	
def argmin(v):
	minv = min(v)
	for i in range(0,len(v)):
		if v[i] == minv:
			return i
	return 0
	
def argmax(v):
	maxv = max(v)
	for i in range(0,len(v)):
		if v[i] == maxv:
			return i
	return 0
	
def remove_noise(x,y):
	x2 = []
	y2 = []
	return x2,y2
	
# Source: http://stackoverflow.com/questions/14720331/how-to-generate-random-colors-in-matplotlib
def get_cmap(N):
    '''Returns a function that maps each index in 0, 1, ... N-1 to a distinct 
    RGB color.'''
    color_norm  = colors.Normalize(vmin=0, vmax=N-1)
    scalar_map = cmx.ScalarMappable(norm=color_norm, cmap='hsv') 
    def map_index_to_rgb_color(index):
        return scalar_map.to_rgba(index)
    return map_index_to_rgb_color
	
	
def statistics(x,y,printStr=""):
	cnt_dist = 0		
	sum1length = sum2length = sum1distance = 0
	diagonal = []
	width = []
	height = []
	length = []
	cnt = len(x)
	it = 0
	while it < cnt:
		if len(x[it]) > 2:
			width.append(abs(max(x[it])-min(x[it])))
			height.append(abs(max(y[it])-min(y[it])))
			diagonal.append(math.sqrt(math.pow(width[-1],2)+math.pow(height[-1],2)))							
			sum1length += len(x[it]) 
			sum2length += pow(len(x[it]) ,2)
			if (it>1 and len(x[it-1])>0):
				sum1distance += abs(max(x[it-1])-min(x[it]))
				cnt_dist +=1
			length.append(len(x[it]))
			it += 1
		else:
			x.pop(it)
			y.pop(it)				
			cnt -= 1

			
	if cnt > 0:
		avg_length    = sum1length/cnt
		dev_length    = math.sqrt(sum2length/(cnt)-pow(avg_length,2))
		avg_height    = sum(height)/cnt
		avg_width     = sum(width)/cnt
		avg_diagonal  = sum(diagonal)/cnt
		avg_distance  = sum1distance/cnt_dist
		overallminx   = min(min(x))
		overallmaxx   = max(max(x))
		overallminy   = min(min(y))
		overallmaxy   = max(max(y))
	else:
		print "No point found, exiting..."

	if printStr is not "":
		print "-------------------------- Stroke statistics " + printStr + "  ----------------------"
		print "\tTotal length\t\t%5d\tNo strokes\t%5d" % (sum1length, cnt)
		print "\tAverage length\t\t%5.0f\tDeviation\t%5.1f" % (avg_length, dev_length)
		print "\tAverage diagonal\t%5.0f\tdistance\t%5.0f" % (avg_diagonal, avg_distance)
	
	return width, height, diagonal, length, avg_length, dev_length, avg_height, avg_width, avg_diagonal,avg_distance, overallminx, overallmaxx, overallminy,overallmaxy,sum1length,cnt

def getLabels(cnt,stroke_vert_right,stroke_vert_left,stroke_vert_undec,stroke_horiz_LtoR,stroke_horiz_RtoL):
	label =  [""] * cnt	
	for it in range(cnt-1):
		if it in stroke_vert_right:
			label[it] = RVERT
		elif it in stroke_vert_left:
			label[it] = LVERT
		elif it in stroke_vert_undec:
			label[it] = UVERT
		elif it in stroke_horiz_RtoL:
			label[it] = LEFT
		elif it in stroke_horiz_LtoR:
			label[it] = RIGHT
	
	return label
	
def plotStrokes(x,y,fileName,plotTitle,t="original-strokes",label=[],x2=[],y2=[],deleted=[]):
	plt.rc('lines', linewidth=1.0)	
	cmap = get_cmap(NC)
	colorit = 0
	origcol = "0.8"	
	if t == "original-strokes":
		for it in range(len(x)):
			tmp = plt.plot(x[it],y[it])
			plt.setp(tmp,color=cmap(colorit))
			colorit += 1
			if colorit > NC: colorit = 0
	elif t == "denoised-strokes":
		for it in range(len(x)):
			tmp = plt.plot(x[it], y[it])
			plt.setp(tmp,color=cmap(colorit),marker="o",linestyle="None",markersize=1.0,markeredgecolor=cmap(colorit))
			colorit += 1
			if colorit >= NC: colorit = 0	
		tmp = plt.plot(x[it][0], y[it][0])
		plt.setp(tmp, markersize=2.0,marker="o",color="w")
		tmp = plt.plot(x[it][-1], y[it][-1])
    		plt.setp(tmp, markersize=2.0,marker="o",color="k")
	elif t == "denoised-annotated" or t  == "annotated":
		for it in range(len(x)):
			tmp = plt.plot(x[it], y[it])
	    		if label[it] == RVERT:
				plt.setp(tmp, color="c", linewidth=3.0)
			elif label[it] == LVERT:
				plt.setp(tmp, color="m", linewidth=3.0)
			elif label[it] == UVERT:
				plt.setp(tmp, color="g", linewidth=3.0)
	    		elif label[it] == LEFT:
				plt.setp(tmp, color="b", linewidth=3.0)
			elif label[it] == RIGHT:
				plt.setp(tmp, color="r", linewidth=3.0)
			else:	
				plt.setp(tmp, color="k", markersize=1.0,marker="o",linestyle="None",markeredgecolor="k")
			tmp = plt.plot(x[it][0], y[it][0])
			plt.setp(tmp, markersize=3.0,marker="o",color="w")
			tmp = plt.plot(x[it][-1], y[it][-1])
	    		plt.setp(tmp, markersize=3.0,marker="o",color="k")

		if t == "annotated":			   
		    	xd = []
		    	yd = []
	    		PINK    = "#ffb6c1"
		    	for it in range(len(deleted)):
		    		xd.append(deleted[it][0])
		    		yd.append(deleted[it][1])
	    		tmp = plt.plot(xd,yd)
			plt.setp(tmp,color=PINK,markersize=3.0,marker="s",linestyle="None",markeredgecolor=PINK)
			
	elif t == "lines":
		noCol = len(label)+1
		cmap = get_cmap(noCol)
		colorit = 0
		for it in range(len(x2)):
			if len(x2[it]) >2:
				tmp = plt.plot(x[it],y[it])
				plt.setp(tmp, color=origcol, markersize=0.5,marker="o",linestyle="None",markeredgecolor=origcol)
				tmp = plt.plot(x2[it],y2[it])	
				plt.setp(tmp,color=cmap(colorit),marker="o",linestyle="None",markersize=1.0,markeredgecolor=cmap(colorit))
			colorit += 1
			if colorit > noCol: colorit = 0

	plt.xlabel("X")
	plt.ylabel("-Y")
	plt.title(plotTitle)
	plt.axes().set_aspect('equal', 'datalim')	# keep 1:1 aspect ratio
	plt.axes().grid(True,which="both",color="0.7")					
	plt.savefig(fileName)
	plt.clf()
	return 1
	
def statistics_one_stroke(x,y):
	width  = abs(max(x)-min(x))
	height = abs(max(y)-min(y))
	diagonal = math.sqrt(math.pow(width,2)+math.pow(height,2))
	length = len(x)
	return width, height, diagonal, length



def point_between_lines(x,y,a1,b1,a2,b2):
	if (x*a1+b1<y and x*a2+b2>y) or  (x*a1+b1>y and x*a2+b2<y):
		return 1
	return 0
	
def list_between_lines(xlist,ylist,a1,b1,a2,b2):
	for i in range(len(xlist)):
		if not point_between_lines(xlist[i],ylist[i],a1,b1,a2,b2):
			return 0
	return 1
	
def centerlist(xlist,ylist):
	if ( len(xlist)>0 and len(xlist) == len(ylist) ):
		cx = sum(xlist)/len(xlist)
		cy = sum(ylist)/len(ylist)
	else:
		cx = cy = 0
	return cx,cy
