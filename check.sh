#!/bin/bash
function grade() {
    cd $PINTOS_HOME
    if [ ! -n "$1" ]; then
        echo "Invalid phase"
    fi

    pintosph $1 > /dev/null &

    cd $1 && make clean && make grade >/dev/null && cd build

    # Check if the file exists
    if [ ! -f "grade" ]; then
        echo "File 'grade' not found."
        exit 1
    fi

    separator="- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "

    # Flag to determine whether to print lines
    print_flag=false
    c=0

    # Read the file line by line
    while IFS= read -r line && [ "$c" -ne 2 ]; do
        # Check for pattern
        if [[ $line == *"$separator"* ]]; then
            if [ $c -eq 0 ]; then
                print_flag=true
                ((c=c+1))
                continue
            else
                print_flag=false
                ((c=c+1))
                continue
            fi
        fi    # Print the first line
        if [ "$print_flag" = false ]; then
            print_flag=true
            continue
        fi
        

        # Print the line if flag is true
        if [ "$print_flag" = true ]; then
            echo "$line"
        fi
    done < "grade"
    echo "more details in $PINTOS_HOME/$1/build/grade"
}
echo "----------------------------------------------------------------------------------"
echo "|                                                                                |"
echo "|                                     Threads                                    |"
echo "|                                                                                |"
echo "----------------------------------------------------------------------------------"
grade threads


echo "----------------------------------------------------------------------------------"
echo "|                                                                                |"
echo "|                                    Userprog                                    |"
echo "|                                                                                |"
echo "----------------------------------------------------------------------------------"
grade userprog

echo "----------------------------------------------------------------------------------"
echo "|                                                                                |"
echo "|                                       VM                                       |"
echo "|                                                                                |"
echo "----------------------------------------------------------------------------------"
grade vm

echo "----------------------------------------------------------------------------------"
echo "|                                                                                |"
echo "|                                    Filesys                                     |"
echo "|                                                                                |"
echo "----------------------------------------------------------------------------------"
grade filesys