# Installing the RadioDoESTower script

This assumes you are running on a Raspberry Pi.

## Installation steps

 * `sudo apt-get install mplayer`
 * `sudo apt-get install ruby`
 * `sudo gem install simple-rss`
 * Configure the scripts by setting the relevant values in `config.yaml`
 * Set up cron to run `download_rss` at regular intervals (e.g. every 15 minutes)
 * Run the `play_radio` script
