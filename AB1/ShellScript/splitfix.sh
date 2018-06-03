#!/bin/bash
#The first shellscript for BSP
#Paul Mathia & Dennis Sentler | paul.mathia@haw-hamburg.de & dennis.sentler@haw-hamburg.de  
#15.04.2018 
#Version 1.0

#Hilfe Funktion zur nur Nutzung von Splitfix.sh
helpFunction()
{
    echo        "Description:      
                splitfix.sh [OPTIONS] FILE [FILE...] Splits FILE into
                fixed size pieces. 
                
                The pieces are 10 lines long, 
                if FILE is a text file.
                
                The pieces are 10kB long, 
                if FILE is *not* a text file. 
                
                The last piece of the splitted file may be smaller,
                it contains the rest of the original file.
                
                The output files bear the name of the input file 
                with a 4 digit numerical suffix.
                
                The original file can be reconstructed with the 
                command ''cat FILE.*''
    
                EXAMPLE: 
                splitfix.sh foo.pdf
                splits foo.pdf into the files foo.pdf0000 foo.pdf0001 etc.
    
                splitfix.sh -h | --help - displays this help text
                splitfix.sh --version  - prints the version number
        
                OPTIONS:
                -h 
                    --help      this help text
    
                -s SIZE         size of the pieces 
                                in lines for a text file
                                or in kBytes for other files
    
                -v
                    --verbose   prints debugging messages"
}

#versionFunktion
version()
{
    echo "$0 Versionsnummer: 1.0"
}

###########################################--- MAIN-Methode ---###########################################
#case: programm ohne parameter aufgerufen
if [ $# -eq 0 ]
    then echo "Programm ohne Parameter aufgerufen!"
    helpFunction
    exit 0
fi

#If version dann anzeigen und raus aus den program
if [ "$1" == "--version" ];
then
	version
	exit 0
fi

#If help dann anzeigen und raus aus den program
if [ "$1" == "--help" ];
then
	helpFunction
	exit 0
fi


#Globale Variablen 
SIZE=10             #standard wert 10 - 10lines(txt) oder 10Kb andere files.
verbose=""       #verbose modus ist anfänglich ausgeschaltet 

#restliche argumente werden verarbeitet
while getopts "hvs:" arg
do
    case $arg in
        h) helpFunction
            ;;
        v) verbose=' --verbose'
            ;;
        s) SIZE=${OPTARG}
            ;;
        *) helpFunction
            ;;
    esac
done

#jetzt ums verarbeiten des Fileparameters kümmern
for i in $*
do
    if  [ -e $1 ]
    then
        file --mime-type $1  |   grep  text/plain   
        if  [ $? -eq 0 ]
        then
            echo "File ist eine Textdatei"
            echo "File wird gesplittet"
            split$verbose -a4 -d --lines="$SIZE" "$1" "$1".
        else
            echo "File ist eine andere Datei"
            echo "File wird gesplittet"
            split$verbose -a4 --bytes="$SIZE"K -d "$1" "$1".
        fi
    fi
    shift
done
exit 0



