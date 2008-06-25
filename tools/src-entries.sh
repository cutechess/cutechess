#!/bin/sh

# Print TODO, FIXME, HACK and NOTE entries from source code files.

# Default settings
print_todos="yes"
print_fixmes="yes"
print_hacks="yes"
print_notes="yes"

print_line_numbers="yes"
print_file_name="yes"

print_entries()
{
	for file in `find . \( ! -regex '.*/\..*' -a -name "*.cpp" -o -name "*.h" \) -type f`
	do
		grep -n -E '^\W*// (TODO|FIXME|HACK|NOTE):' $file | while read entry
		do
			local line_num=`echo "$entry" | cut -f 1 -d :`
			local entry_type=`echo "$entry" | cut -f 2 -d : | sed -e 's/^\W*//g'`
			local entry_text="`echo $entry | cut -f 3 -d : | sed -e 's/^\W*//g'`"

			# Should we print this type of entry?
			local print_this_type="yes"

			case $entry_type in
				TODO )
					if [ $print_todos = "no" ]; then
						print_this_type="no"
					fi
				;;

				FIXME )
					if [ $print_fixmes = "no" ]; then
						print_this_type="no"
					fi
				;;

				HACK )
					if [ $print_hacks = "no" ]; then
						print_this_type="no"
					fi
				;;

				NOTE )
					if [ $print_notes = "no" ]; then
						print_this_type="no"
					fi
				;;
			esac

			if [ $print_this_type = "yes" ]
			then
				if [ $print_file_name = "no" ]
				then
					echo "$entry_type: $entry_text"
				else
					if [ $print_line_numbers = "no" ]
					then
						echo "$file: $entry_type: $entry_text"
					else
						echo "$file:$line_num: $entry_type: $entry_text"
					fi
				fi
			fi
		done
	done
}

print_usage()
{
	echo "Usage: src-entries.h [options]"
	echo ""
	echo "Print TODO, FIXME, HACK and NOTE entries from source code files. All"
	echo "entries are printed by default with the file name and line number."
	echo ""
	echo "The entries must be in format:"
	echo ""
	echo "    <whitespace>// [TODO|FIXME|HACK|NOTE]: <text>"
	echo ""
	echo "Only one entry per line (text included) is currently supported."
	echo ""
	echo "Options:"
	echo ""
	echo "    --no-file-name     don't print the file name where the entry came from"
	echo "    --no-line-number   don't print the line number where the entry came from"
	echo "    --no-todos         don't print TODO entries"
	echo "    --no-fixmes        don't print FIXME entries"
	echo "    --no-hacks         don't print HACK entries"
	echo "    --no-notes         don't print NOTE entries"
	echo ""
	exit 0
}

while [ $# -ge 1 ]
do
	case $1 in
		--no-file-name )
			print_file_name="no"

			# Disable printing line number too as it's pretty much useless
			# without the file name
			print_line_numbers="no"
		;;
		--no-line-number )
			print_line_numbers="no"
		;;
		--no-todos )
			print_todos="no"
		;;
		--no-fixmes )
			print_fixmes="no"
		;;
		--no-hacks )
			print_hacks="no"
		;;
		--no-notes )
			print_notes="no"
		;;
		* )
			print_usage
		;;
	esac

	shift
done

print_entries

exit 0
