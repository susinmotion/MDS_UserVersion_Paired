import gzip
from subprocess import Popen, PIPE

def main():
        with open("config.cfg", 'rb') as config:
                lines= config.readlines()
                if lines[0].split("=")[1]=="0":
                        zipped=False
                else:
                        zipped=True
                filenames=lines[1].split("=")[1].split(",")
		barcodeLength=int(lines[3].split()[1])
                forwardAligns=lines[4].split()[1].split(",")
                reverseAligns=lines[5].split()[1].split(",")
                targets=lines[6].split()[1].split(",")
                phaseShifts=[]
                for phase in lines[9].split()[1].split(","):
                        shifts_one_phase=[]
                        for shift in phase.split("|"):
                                shifts_one_phase.append( (shift[0],shift[2]) )
                        phaseShifts.append(shifts_one_phase)


                filesets=[]
                for i in xrange(0,len(filenames),2):
                        filesets.append( [filenames[i].strip("\n"), filenames[i+1].strip("\n")])

                zippedCall="gzip.open('{}','rB')"
                unzippedCall="open('{}', 'rB')"
                if zipped:
                        call=zippedCall
                else:
                        call=unzippedCall
                for fileset in filesets:
                        with eval(call.format(fileset[0])) as f:
                                with eval(call.format(fileset[1])) as g:
                                        print "Processing files {} and {}".format(fileset[0],fileset[1])
                                        stillWorking=True
                                        while stillWorking:

                                                read1=f.readline()
                                                read2=g.readline()

                                                read1=f.readline().strip("\n")
                                                read2=g.readline().strip("\n")
                                                if not (read1 and read2):
                                                        print -100
                                                        break
                                                sequence1=""
                                                sequence2=""
                                                barcode1=""
                                                barcode2=""
                                                phase1=None
                                                phase2=None
                                                phase=None

                                                for i in range(len(targets)):
                                                        sequence=read1
                                                        indexForwardAlign=sequence.find(forwardAligns[i],barcodeLength)#find forward and reverse alignment sequences
                                                        indexReverseAlign=sequence.find(reverseAligns[i],barcodeLength+len(forwardAligns[i] ));
                					if ((indexForwardAlign != -1) and (indexReverseAlign != -1) ):
                                                                targetNo1=i
                                                                barcode1=sequence[(indexForwardAlign-barcodeLength):(indexForwardAlign)]
                                                                phase1 = indexForwardAlign-barcodeLength;
                                                                sequence1=sequence[(indexForwardAlign+len(forwardAligns[i]) ):(indexReverseAlign)]


                	                        for i in range(len(targets)):
                                                        sequence=read2
                                                        indexReverseAlign=sequence.find(reverseComplement(reverseAligns[i]))
                                                        indexForwardAlign=sequence.find(reverseComplement(forwardAligns[i]))


                                                        if ((indexForwardAlign != -1) and (indexReverseAlign != -1) ):
                                                                targetNo2=i
                                                                barcode2= reverseComplement(sequence[indexForwardAlign+len(forwardAligns[i])-1:(indexForwardAlign+len(forwardAligns[i])+barcodeLength) ])
                                                                phase2= indexReverseAlign
                                                                sequence2=reverseComplement(sequence[(indexReverseAlign+len(reverseAligns[i])-1):(indexForwardAlign)])

                                                if sequence1 and len(barcode1)==barcodeLength and len(barcode2)==barcodeLength:
                                                        if barcode1==barcode2 and targetNo1==targetNo2:
                                                                for i in range(len(phaseShifts[targetNo1])):
                                        
                                                                        if str(phase1)==phaseShifts[targetNo1][i][0] and str(phase2)==phaseShifts[targetNo1][i][1]:
                                                                                phase=i
                                                
                                                                if phase:
                                                                        finalSequence=barcode1+str(phase)+forwardAligns[targetNo1]+targets[targetNo1]+reverseAligns[targetNo1]
                                                                        print targetNo1, phase, barcode1,sequence1
                                                                        print targetNo2, phase, barcode2,sequence2
                                                else:
                                                        continue
                                                        #print sequence
                                                f.readline()
                                                g.readline()
                                                f.readline()
                                                g.readline()
                print -1000

def reverseComplement(sequence):
        complements={'A':'T','T':'A','G':'C','C':'G','N':'N'}
        reverseComplement=""
        for i in range(len(sequence)-1, 0, -1):
                reverseComplement=reverseComplement+complements[sequence[i]]
        return reverseComplement

if __name__ == "__main__":
        main()
