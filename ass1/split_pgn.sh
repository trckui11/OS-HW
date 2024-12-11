# Check for number of arguments
if [ ! $# -eq 2 ]; then
    echo "Usage: $0 <source_pgn_file> <destination_directory>"
    exit 1
fi

# Check if source file exists
if [ ! -e $1 ]; then
    echo "Error: File '$1' does not exist."
    exit 1
fi

# Create directory if necessary
if [ ! -d $2 ]; then
    mkdir $2
    echo "Created directory '$2'."
fi

# Split file
prev=1
index=0
# Iterate through first lines of new games
for line in $(grep -n "\[Event " $1 | cut -d: -f1); do
    if [ $index -eq 0 ]; then
        ((index++))
        prev=$line
        continue
    fi
    fileName=${1/.pgn/_${index}.pgn}
    head -n $[$line-2] $1 | tail -n $[$line-$prev-1] > $fileName
    mv $fileName $2
    echo Saved game to $2/$fileName
    prev=$line
    ((index++))
done
# Last game
length=$(wc -l < "$1")
lastFile=${1/.pgn/_${index}.pgn}
tail -n $[$length-$prev+1] $1 > $lastFile
mv $lastFile $2
echo Saved game to $2/$lastFile
echo All games have been split and saved to \'$2\'.
