# Installing the RadioDoESTower script

This assumes you are running on a Raspberry Pi.

## Installation steps

 * `sudo apt-get install mplayer`
 * `sudo apt-get install ruby`
 * `sudo apt-get install ruby-dev`
 * `sudo gem install nokogiri`
 * Configure the scripts by setting the relevant values in `config.yaml`
 * Set up cron to run `download_rss` at regular intervals (e.g. every 20 minutes), run `crontab -e` and add:
    */20 * * * * cd /home/pi/Applications/beaconbot/RadioDoESTower && ./download_rss
 * Set the `play_radio` script to run at boot time, run `sudo vi /etc/rc.local` and add:
    sudo -u pi screen -S radio -d -m /home/pi/Applications/beaconbot/RadioDoESTower/loop_play_radio
    
