#!/bin/bash

# To run this bashscipt type ytdlp in terminal.
# This bashscipt downloads the video in the youtube url, and then saves in /home/Videos/directoryname, where directoryname is given by the user
# For more info, go the page https://github.com/yt-dlp/yt-dlp and gitpush.sh

# Check if yt-dlp is installed
if ! command -v yt-dlp &> /dev/null; then
    echo "yt-dlp is not installed. Please install it and try again."
    exit 1
fi
# To install yt-dlp run the command --> sudo pacman -Syu ytdlp

# Function to download a YouTube video in video format
download_video() {
    read -p "Enter the YouTube video URL: " video_url

    echo "Select video quality:"
    echo "1. 480p"
    echo "2. 720p"
    echo "3. 1080p"
    echo "4. 2160p"
    read -p "Enter your choice: " quality_choice

    case $quality_choice in
        1) quality="best[height<=480]" ;;
        2) quality="best[height<=720]" ;;
        3) quality="best[height<=1080]" ;;
        4) quality="best[height<=2160]" ;;
        *) 
            echo "Invalid choice. Exiting."
            exit 1
            ;;
    esac

    read -p "Enter the directory name for saving the video: " directory_name
    save_path="$HOME/Videos/$directory_name"
    mkdir -p "$save_path"

    yt-dlp -f "$quality" -o "$save_path/%(title)s.%(ext)s" "$video_url"
}

# Function to download a YouTube video in audio format
download_audio() {
    read -p "Enter the YouTube video URL: " video_url
    read -p "Enter the directory name for saving the audio file: " directory_name

    save_path="$HOME/Videos/$directory_name"
    mkdir -p "$save_path"

    yt-dlp -x --audio-format mp3 -o "$save_path/%(title)s.%(ext)s" "$video_url"
}

# Function to download a YouTube playlist
download_playlist() {
    read -p "Enter the YouTube playlist URL: " playlist_url
    read -p "Enter the directory name for saving the playlist: " directory_name

    save_path="$HOME/Videos/$directory_name"
    mkdir -p "$save_path"

    yt-dlp -o "$save_path/%(playlist_title)s/%(title)s.%(ext)s" "$playlist_url"
}

# Main script
echo "Welcome to YouTube Downloader"
echo "Choose an option:"
echo "1. Download a YouTube video (Video format)"
echo "2. Download a YouTube video (Audio format)"
echo "3. Download a YouTube playlist"
read -p "Enter your choice: " option

case $option in
    1) download_video ;;
    2) download_audio ;;
    3) download_playlist ;;
    *) 
        echo "Invalid choice. Exiting."
        exit 1
        ;;
esac

# Name this file as ytdlp.sh and then move to local/bin using the command --> mv ytdlp.sh ~/.local/bin/ytdlp
# Give permissions using the command --> chmod +x ~/.local/bin/ytdlp
# Now it is executable from anywhere