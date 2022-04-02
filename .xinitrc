# xrandr config
xrandr --output HDMI-1 --right-of DP-2 --rotate left
xrandr --output HDMI-0 --left-of DP-2 --rotate left 
xrandr --output DP-2 --primary --pos 1080x240

# This fixes barpadding issue where rotated monitor windows don't respect
# vertical padding properly
#xrandr --output HDMI-0 --rotate normal --above DP-2
#xrandr --output HDMI-0 --left-of DP-2 --pos 0x0

# Load background with feh
~/.fehbg &

# Load dwm-blocks
dwmblocks &

# Start picom with kawase blur on glx experimental enabled
picom --experimental-backends &

# Disable sleep and display power manager for X11
xset -dpms
xset s 0 0

# Start window manager
exec dwm

# echo "$(date): Starting dwm" >> /home/$USER/.logs/dwm/$(date +%Y-%m-%d_%H.dwm.log)
# exec dwm >> /home/$USER/.logs/dwm/$(date +%Y-%m-%d_%H.dwm.log) 2>&1
