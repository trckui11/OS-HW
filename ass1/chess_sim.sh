# Check if source file exists
if [ ! -e "$1" ]; then
    echo "File does not exist: $1"
    exit 1
fi

# Print metadata
echo "Metadata from PGN file:"
line=$(grep -n "1\. " "$1" | cut -d: -f1 | head -1)
head -n $((line-1)) "$1"

# Extract moves
length=$(wc -l < "$1")
tail -n $(($length-$line+1)) $1 > "moves_file"
python3 parse_moves.py "$(cat "moves_file")" > "moves_file"

# Find number of moves
num_of_moves=$(cat "moves_file" | wc -w)

# Initialize board with table
declare -A board
initialize_board() {
    board=(
    [a8]="r" [b8]="n" [c8]="b" [d8]="q" [e8]="k" [f8]="b" [g8]="n" [h8]="r"
    [a7]="p" [b7]="p" [c7]="p" [d7]="p" [e7]="p" [f7]="p" [g7]="p" [h7]="p"
    [a6]="." [b6]="." [c6]="." [d6]="." [e6]="." [f6]="." [g6]="." [h6]="."
    [a5]="." [b5]="." [c5]="." [d5]="." [e5]="." [f5]="." [g5]="." [h5]="."
    [a4]="." [b4]="." [c4]="." [d4]="." [e4]="." [f4]="." [g4]="." [h4]="."
    [a3]="." [b3]="." [c3]="." [d3]="." [e3]="." [f3]="." [g3]="." [h3]="."
    [a2]="P" [b2]="P" [c2]="P" [d2]="P" [e2]="P" [f2]="P" [g2]="P" [h2]="P"
    [a1]="R" [b1]="N" [c1]="B" [d1]="Q" [e1]="K" [f1]="B" [g1]="N" [h1]="R")
}

# Print the board. Receive the current move
display_board() {
    echo "Move $1/$num_of_moves"
    echo "  a b c d e f g h"
    for number in {8..1}; do
        echo -n "$number "
        for letter in {a..h}; do
            echo -n "${board[$letter$number]} "
        done
        echo -n "$number"
    echo
    done
    echo "  a b c d e f g h"
}

# Plays one move on the board
single_move() {
    local move=$1
    local from=${move:0:2}
    local to=${move:2:2}
    local piece=${board[$from]}
    board[$to]=$piece
    board[$from]='.'
}

# Handle promotion
promotion() {
    local move=$1
    local from=${move:0:2}
    local to=${move:2:2}
    local promoted_piece=${move:4:1}
    board[$to]=$promoted_piece
    board[$from]='.'
}

# Handle castling
castling() {
    local move=$1

    # Make sure the move is actually castling and perform the move
    case "$move" in
        "e1g1")
            if [ "${board[e1]}" == "K" ] && [ "${board[h1]}" == "R" ] && [ "${board[f1]}" == "." ] && [ "${board[g1]}" == "." ]; then
                # White kingside castling
                board[g1]="K"
                board[f1]="R"
                board[e1]='.'
                board[h1]='.'
                return
            fi
            ;;
        "e1c1")
            if [ "${board[e1]}" == "K" ] && [ "${board[a1]}" == "R" ] && [ "${board[b1]}" == "." ] && [ "${board[c1]}" == "." ] && [ "${board[d1]}" == "." ]; then
                # White queenside castling
                board[c1]="K"
                board[d1]="R"
                board[e1]='.'
                board[a1]='.'
                return
            fi
            ;;
        "e8g8")
            if [ "${board[e8]}" == "k" ] && [ "${board[h8]}" == "r" ] && [ "${board[f8]}" == "." ] && [ "${board[g8]}" == "." ]; then
                # Black kingside castling
                board[g8]="k"
                board[f8]="r"
                board[e8]='.'
                board[h8]='.'
                return
            fi
            ;;
        "e8c8")
            if [ "${board[e8]}" == "k" ] && [ "${board[a8]}" == "r" ] && [ "${board[b8]}" == "." ] && [ "${board[c8]}" == "." ] && [ "${board[d8]}" == "." ]; then
                # Black queenside castling
                board[c8]="k"
                board[d8]="r"
                board[e8]='.'
                board[a8]='.'
                return
            fi
            ;;
    esac
    single_move $move
}

en_passant() {
    local move=$1
    local from=${move:0:2}
    local to=${move:2:2}
    local end_col=${to:0:1}
    local end_row=${to:1:1}
    local piece=${board[$from]}

    # Check if the move is actually an en passant
    if [[ "$piece" == "P" && "${board[$to]}" == "." && "${board[${end_col}$((end_row-1))]}" == "p" ]]; then
        # White pawn en passant
        board[$from]='.'
        board[$to]="P"
        board[${end_col}$((end_row-1))]='.'
    elif [[ "$piece" == "p" && "${board[$to]}" == "." && "${board[${end_col}$((end_row+1))]}" == "P" ]]; then
        # Black pawn en passant
        board[$from]='.'
        board[$to]="p"
        board[${end_col}$((end_row+1))]='.'
    else
        # Not really en passant
        single_move $move
    fi
}

# Update the board to show the desired move and display it
play_till_move() {
    local current_move=$1
    initialize_board

    # Play moves up to the current move
    local moves=$(cat "moves_file")
    i=0
    for move in $moves; do
        ((i++))
        if [ $i -le $current_move ]; then
            if [[ $move == e1g1 || $move == e1c1 || $move == e8g8 || $move == e8c8 ]]; then
                castling $move
            elif [ ${#move} -eq 5 ]; then
                promotion $move
            elif [[ $move =~ ^[a-h][2-7][a-h][3-6]$ ]]; then
                en_passant $move
            else
                single_move $move
            fi
        else
            break
        fi
    done
    display_board $current_move
}

# Start game
current=0
initialize_board
display_board $current
while true; do
    echo -n "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit:"
    read action
    echo
    case "$action" in
        "a")
            if [ $current -gt 0 ]; then
                ((current--))
            fi
            ;;
        "d")
            if [ $current -lt $num_of_moves ]; then
                ((current++))
            else
                echo "No more moves available."
                continue
            fi
            ;;
        "s")
            current=$num_of_moves
            ;;
        "w")
            current=0
            ;;
        "q")
            echo -e "Exiting.\nEnd of game."
            break
            ;;
        *)
            echo "Invalid key pressed: $action"
            continue
            ;;
    esac
    play_till_move $current
done

rm "moves_file"