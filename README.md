# Wallpaper Splitter

On KDE it is not possible to apply an image so that it spans across all of your screens.
This tool fixes that by splitting your image according to your screen setup.
It can also directly apply the image as your wallpaper.
![img.png](docs/img.png)

Keep in mind that this only works with images, not with any fancy wallpaper engine or even dynamic wallpapers.


## 🚀 Features and roadmap

This is only an oveeview. For a more in-depth look see the project tab. 
- [x] Split a given image
- [x] Apply the wallpaper from within the application
- [x] Adjust position
- [x] Adjust scale*
- [ ] Zooming into the scene and moving it around with the mouse wheel (scroll / click)
- [x] Command line tool
- [x] Support drag 'n drop

*there is a bug with diagonal scaling

## 💭 How to use it

_Some of these features might not be implemented yet._

1. Click <kbd>📂 Open</kbd> to select your image.
2. Adjust the position of your screens with <kbd>Left 🖱️</kbd> and the size with <kbd>Right 🖱️</kbd>.
   You can also zoom and move the scene around with your mouse wheel.
3. Save the images that will be your wallpaper by clicking <kbd>💾 Save</kbd> or
   apply them directly by clicking <kbd>✔️ Ok</kbd>.

Note that the buttons in the screenshot are labeled in german since that is my system language.


## ⚙️ How does it work

Opening and splitting the image is pretty straight forward.
Applying the image is done via a dbus call to the Plasma Shell,
for more on that see their documentation provided [here](https://develop.kde.org/docs/plasma/scripting/api/).


## 💡 How to help

If you want to help out, create an issue or pull request and I will have a look at it.
If you are able to implement these features directly into plasma, I would love to see that!
