#!/bin/bash

first_file="$1"

if [[ ! -f "$first_file" ]]; then
  echo "Исходный файл '$first_file' отсутствует" >&2
  exit 1
fi

temp_dir=$(mktemp -d)
trap "rm -rf '$temp_dir'; exit" EXIT SIGINT SIGTERM
cp "$first_file" "$temp_dir"
output_file=$(grep -Eo '&Output: ([^ ]+)' "$temp_dir/$first_file" | awk '{print $2}')

if [[ -z "$output_file" ]]; then
  echo "В исходном файле отсутствует комментарий &Output:" >&2
  exit 2
fi

if [[ "$first_file" =~ \.c$ ]]; then
  gcc "$temp_dir/$first_file" -o "$temp_dir/$output_file"
elif [[ "$first_file" =~ \.cpp$ ]]; then
  g++ "$temp_dir/$first_file" -o "$temp_dir/$output_file"
elif [[ "$first_file" =~ \.tex$ ]]; then
  pdflatex "$temp_dir/$first_file"
fi

if [[ $? -ne 0 ]]; then
  echo "Сборка не удалась" >&2
  exit 3
fi

mv "$temp_dir/$output_file" .
rm -rf "$temp_dir"
echo "Сборка выполнена"
