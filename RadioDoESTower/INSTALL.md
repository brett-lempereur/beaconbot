# Installing the RadioDoESTower script

This assumes you are running on a Raspberry Pi.

## Installation steps

 * `sudo apt-get install mplayer`
 * `sudo apt-get install ruby`
 * Install and set-up the [Dropbox uploader](https://github.com/andreafabrizi/Dropbox-Uploader) script (which we use to download)
    git clone https://github.com/andreafabrizi/Dropbox-Uploader/
    cd Dropbox-Uploader
    ./dropbox_uploader.sh
 * Configure dropbox_uploader to a Dropbox app you create, then set up a cron job to regularly synchronise and download files
 * Set the folder that Dropbox is synchronising to be the one listed in play_radio in the `music_dir` variable.
 * Run the `play_radio` script
