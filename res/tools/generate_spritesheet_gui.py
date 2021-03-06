#!/usr/bin/env python3
import tkinter as tk
import tkinter.filedialog
import argparse
from PIL import ImageTk, Image
import os
import cv2
import numpy as np
import glob


# Sprite Sheet mechanics

class myImageSprite:
    # This is the internal class where I will store images for manipulation within my sprite gui code
    # we have the following
    # base_image,filtered_image
    def __init__(self, x=200, p=4, file_name='your_mom'):

        self.base_image = []
        self.filtered_image = []
        self.cell_size = x
        self.padding = p
        self.file_name = file_name
        if file_name != 'your_mom':
            self.loadImage(file_name)

    def imageSize(self):
        return self.cell_size - 2 * self.padding

    def imageStartX(self):
        return self.padding

    def imageEndX(self):
        return self.cell_size - self.padding

    def loadImage(self, file_name):
        self.file_name = file_name
        local_image = cv2.imread(file_name)
        if local_image.shape[2] == 3:
            b_channel, g_channel, r_channel = cv2.split(local_image)
            alpha_channel = np.ones(b_channel.shape,
                                    dtype=b_channel.dtype) * 255  # creating a dummy alpha channel image.
            local_image = cv2.merge((b_channel, g_channel, r_channel, alpha_channel))
        self.base_image = local_image
        self.filtered_image = local_image.copy()

    def outputImage(self):
        final_image = np.zeros((self.cell_size, self.cell_size, 4), dtype=np.uint8)
        final_image[self.imageStartX():self.imageEndX(), self.imageStartX():self.imageEndX(), :] = \
            cv2.resize(self.filtered_image, (self.imageSize(), self.imageSize()), interpolation=cv2.INTER_AREA)
        return final_image


class Error(Exception):
    """Base class for other exceptions"""
    pass


class TooManyImages(Error):
    """Raised when there are too many images for the size spritemap"""
    pass


def gen_sprite_sheet(sprite_list: [myImageSprite], output_file: str, cell_size: int, n_col: int):
    total_size = cell_size * n_col
    index_y = 0
    index_x = 0
    final_image = np.zeros((total_size, total_size, 4), dtype=np.uint8)
    for sprite in sprite_list:
        if (index_x == n_col):
            raise TooManyImages
        current_y = index_x * cell_size
        current_x = index_y * cell_size
        final_image[current_y:(current_y + cell_size), current_x:(current_x + cell_size), :] = sprite.outputImage()
        index_y += 1
        if (index_y == n_col):
            index_y = 0
            index_x += 1
        cv2.imwrite(output_file, final_image)


# Gui Mechanics
class Menubar(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.menubar = tk.Menu(controller)
        self.parent = parent
        self.controller = controller
        self.parent.save_file = ''
        self.filemenu = tk.Menu(self.menubar, tearoff=0)
        self.filemenu.add_command(label="Open", command=self.load)
        self.filemenu.add_command(label="Save", command=self.save)
        self.filemenu.add_command(label="Save as...", command=self.save_as)
        self.filemenu.add_separator()
        self.filemenu.add_command(label="Refresh Tools", command=self.new_toolbox)
        self.filemenu.add_command(label="Output", command=self.change_output_file)
        self.filemenu.add_command(label="Exit", command=self.parent.quit)
        self.menubar.add_cascade(label="File", menu=self.filemenu)

    def save_as(self):
        files = [('Sprite Map Files', '*.txt')]
        f = tk.filedialog.asksaveasfile(mode="w", filetypes=files, defaultextension=files)
        if not f:
            return
        self.controller.save_file = f.name
        f.close()
        self.save()

    def save(self):
        if (self.controller.save_file == ''):
            self.save_as()
        else:
            f = open(self.controller.save_file, "w")
            f.write(str(self.controller.cell_width) + "\n")
            f.write(str(self.controller.padding) + "\n")
            f.write(str(self.controller.output_file) + "\n")
            for sprite in self.controller.my_sprites:
                f.write(sprite.file_name + "\n")
            f.close()

    def load(self):
        fn = tk.filedialog.askopenfilename(title="Select Sprite Map Save",
                                           filetypes=(("Sprite Map files", "*.txt"),))
        if not fn:
            return
        fin = os.path.join(fn)
        self.controller.my_sprites = []
        f = open(fin, "r")
        f1 = f.readlines()
        self.controller.cell_width = int(f1[0].rstrip('\n'))
        self.controller.padding = int(f1[1].rstrip('\n'))
        self.controller.output_file = (f1[2].rstrip('\n'))
        self.controller.file_list = []
        for i in range(3, len(f1)):
            self.controller.file_list.append(f1[i].rstrip('\n'))
        f.close()
        self.parent.redraw()
        self.parent.tool_window.refresh_listbox()

    def change_output_file(self):
        fn = tk.filedialog.askopenfilename(title="Select Sprite Map Output File", filetypes=(("PNG", "*.png"),))
        if not fn:
            return
        self.controller.output_file = os.path.join(fn)
        self.parent.redraw()

    def new_toolbox(self):
        self.parent.tool_window = ToolWindow(self.parent, self.controller)


# Note: If we decide that we only want to be able to select a single image, then I can make it show the image you are moving in another box.
# I cannot figure out a way to show the image next to the image without essentially rewriting the listbox class.
class ReorderableListbox(tk.Listbox):
    """ A Tkinter listbox with drag & drop reordering of lines """

    # Modded from https://stackoverflow.com/questions/14459993/tkinter-listbox-drag-and-drop-with-python
    def __init__(self, master, controller, **kw):
        kw['selectmode'] = tk.EXTENDED
        tk.Listbox.__init__(self, master, kw)
        self.controller = controller
        self.parent = master.parent
        self.bind('<Button-1>', self.setCurrent)
        self.bind('<Control-1>', self.toggleSelection)
        self.bind('<B1-Motion>', self.shiftSelection)
        self.bind('<Leave>', self.onLeave)
        self.bind('<Enter>', self.onEnter)
        self.selectionClicked = False
        self.left = False
        self.unlockShifting()
        self.ctrlClicked = False

    def orderChangedEventHandler(self):
        pass

    def onLeave(self, event):
        # prevents changing selection when dragging
        # already selected items beyond the edge of the listbox
        if self.selectionClicked:
            self.left = True
            return 'break'

    def onEnter(self, event):
        self.left = False

    def setCurrent(self, event):
        self.ctrlClicked = False
        i = self.nearest(event.y)
        self.selectionClicked = self.selection_includes(i)
        if (self.selectionClicked):
            return 'break'

    def toggleSelection(self, event):
        self.ctrlClicked = True

    def moveElement(self, source, target):
        if not self.ctrlClicked:
            element = self.get(source)
            self.delete(source)
            self.insert(target, element)
        self.controller.file_list = self.get(0, self.size())
        self.parent.redraw()

    def unlockShifting(self):
        self.shifting = False

    def lockShifting(self):
        # prevent moving processes from disturbing each other
        # and prevent scrolling too fast
        # when dragged to the top/bottom of visible area
        self.shifting = True

    def shiftSelection(self, event):
        if self.ctrlClicked:
            return
        selection = self.curselection()
        if not self.selectionClicked or len(selection) == 0:
            return

        selectionRange = range(min(selection), max(selection))
        currentIndex = self.nearest(event.y)

        if self.shifting:
            return 'break'

        lineHeight = 15
        bottomY = self.winfo_height()
        if event.y >= bottomY - lineHeight:
            self.lockShifting()
            self.see(self.nearest(bottomY - lineHeight) + 1)
            self.master.after(500, self.unlockShifting)
        if event.y <= lineHeight:
            self.lockShifting()
            self.see(self.nearest(lineHeight) - 1)
            self.master.after(500, self.unlockShifting)

        if currentIndex < min(selection):
            self.lockShifting()
            notInSelectionIndex = 0
            for i in selectionRange[::-1]:
                if not self.selection_includes(i):
                    self.moveElement(i, max(selection) - notInSelectionIndex)
                    notInSelectionIndex += 1
            currentIndex = min(selection) - 1
            self.moveElement(currentIndex, currentIndex + len(selection))
            self.orderChangedEventHandler()
        elif currentIndex > max(selection):
            self.lockShifting()
            notInSelectionIndex = 0
            for i in selectionRange:
                if not self.selection_includes(i):
                    self.moveElement(i, min(selection) + notInSelectionIndex)
                    notInSelectionIndex += 1
            currentIndex = max(selection) + 1
            self.moveElement(currentIndex, currentIndex - len(selection))
            self.orderChangedEventHandler()
        self.unlockShifting()
        return 'break'


class ToolWindow(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.parent = parent
        self.controller = controller
        self.filewin = tk.Toplevel(self.controller)
        self.filewin.geometry("700x1000")
        # stuff that should be markup
        self.filewin.top_side = tk.Frame(self.filewin)
        self.filewin.top_side_1 = tk.Frame(self.filewin)
        self.filewin.top_side_2 = tk.Frame(self.filewin)
        self.filewin.bottom_side = tk.Frame(self.filewin)
        self.label_1 = tk.Label(self.filewin.top_side, text="Sprite Size", fg='blue')
        self.entry_1 = tk.Entry(self.filewin.top_side, text="200", fg='red', bd=3)
        self.label_2 = tk.Label(self.filewin.top_side_1, text="Padding Size", fg='blue')
        self.entry_2 = tk.Entry(self.filewin.top_side_1, text="4", fg='red', bd=3)
        self.label_3 = tk.Label(self.filewin.top_side_2, text="Number of Columns", fg='blue')
        self.entry_3 = tk.Entry(self.filewin.top_side_2, text="5", fg='red', bd=3)
        self.button_res = tk.Button(self.filewin.bottom_side, text="redraw image", command=self.redraw_images)
        self.button = tk.Button(self.filewin.bottom_side, text="import sprite", command=self.open_picture)
        for ent in [self.entry_1, self.entry_2, self.entry_3]:
            ent.delete(0, 'end')
        self.entry_1.insert(tk.END, str(self.controller.cell_width))
        self.entry_2.insert(tk.END, str(self.controller.padding))
        self.entry_3.insert(tk.END, str(self.controller.n_col))
        self.filewin.top_side.pack(fill=tk.X)
        self.filewin.top_side_1.pack(fill=tk.X)
        self.filewin.top_side_2.pack(fill=tk.X)
        self.label_1.pack(side=tk.LEFT, padx=10, pady=0)
        self.entry_1.pack(side=tk.RIGHT, fill=tk.X, padx=10, pady=0)
        self.label_2.pack(side=tk.LEFT, padx=10, pady=0)
        self.entry_2.pack(side=tk.RIGHT, fill=tk.X, padx=10, pady=0)
        self.label_3.pack(side=tk.LEFT, padx=10, pady=0)
        self.entry_3.pack(side=tk.RIGHT, fill=tk.X, padx=10, pady=0)
        self.button_res.pack(fill=tk.X, padx=10, pady=0)
        self.button.pack(fill=tk.X, padx=10, pady=0)

        self.filewin.bottom_side.pack(fill=tk.BOTH, expand=True)
        self.filewin.bottom_side.parent = self.parent
        self.listbox = ReorderableListbox(self.filewin.bottom_side, controller)
        self.color_bar = 0
        for i, name in enumerate(self.controller.file_list):
            self.listbox.insert(tk.END, name)
            if i % 2 == 0:
                self.listbox.selection_set(i)
            self.color_bar = i
        self.listbox.pack(fill=tk.BOTH, expand=True)

    def redraw_images(self):
        self.controller.n_col = int(self.entry_3.get())
        self.controller.padding = int(self.entry_2.get())
        self.controller.cell_width = int(self.entry_1.get())
        self.parent.redraw()
        self.parent.tool_window.refresh_listbox()

    def refresh_listbox(self):
        self.listbox.delete(0, tk.END)
        self.color_bar = 0
        for i, name in enumerate(self.controller.file_list):
            self.listbox.insert(tk.END, name)
            if i % 2 == 0:
                self.listbox.selection_set(i)
            self.color_bar = i
        self.listbox.pack(fill=tk.BOTH, expand=True)

    def open_picture(self):
        f = tk.filedialog.askopenfilenames(title="Select Sprite", filetypes=(
            ("All Images", "*.png | *.jpg | *.JPG"), ("png files", "*.png"), ("all files", "*.*")))
        if not f:
            return
        for f_x in f:
            self.controller.file_list.append(f_x)
            self.listbox.insert(tk.END, self.controller.file_list[-1])
            self.color_bar = self.color_bar + 1
            if self.color_bar % 2 == 0:
                self.listbox.selection_set(self.color_bar)
            self.added_image()

    def added_image(self):
        self.controller.my_sprites.append(myImageSprite(x=self.controller.cell_width, p=self.controller.padding,
                                                        file_name=self.controller.file_list[-1]))
        self.parent.redraw()

class MainApplication(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        tk.Frame.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.parent.title('Sprite Organizer')
        self.menubar = Menubar(self, parent)
        self.parent.config(menu=self.menubar.menubar)
        self.tool_window = ToolWindow(self, parent)

    def resize(self, event):
        self.parent.image = self.parent.image_copy.resize((event.width - 2, event.height - 2), Image.ANTIALIAS)
        self.parent.background_image = ImageTk.PhotoImage(self.parent.image)
        self.parent.the_sprites.configure(image=self.parent.background_image)

    def redraw(self):
        self.parent.my_sprites = [myImageSprite(x=self.parent.cell_width, p=self.parent.padding, file_name=f_name) for
                                  f_name in self.parent.file_list]
        gen_sprite_sheet(self.parent.my_sprites, self.parent.output_file, self.parent.cell_width, self.parent.n_col)
        self.parent.image = Image.open(self.parent.output_file)
        self.parent.image_copy = self.parent.image.copy()
        self.parent.background_image = ImageTk.PhotoImage(self.parent.image)
        self.parent.the_sprites.configure(image=self.parent.background_image)


# Parser Mechanics
parser = argparse.ArgumentParser()
parser.add_argument('--output', '-o', type=str, default='./res/textures/voxel.png',
                    help='Output file to write combined spritesheet')
parser.add_argument('--input', '-i', type=str, default='./res/textures/sprites',
                    help='Directory to read textures from (reads sorted order based on name and adds to sheet)')
parser.add_argument('--padding', '-p', type=int, default=4,
                    help='Number of pixels to add as padding around each texture')
parser.add_argument('--gui', '-g', type=int, default=1, help="Use the GUI instead")
parser.add_argument('--n_columns', '-c', type=int, default=5, help='Number of columns to use')
parser.add_argument('--cell_width', '-w', type=int, default=200, help='How many pixels should sprite be')
args = parser.parse_args()

if args.gui == 1:
    # Gui loop
    root = tk.Tk()
    root.cell_width = args.cell_width
    root.padding = args.padding
    root.n_col = args.n_columns
    root.output_file = args.output
    root.file_list = []
    root.my_sprites = []
    root.image = Image.open(root.output_file)
    root.image_copy = root.image.copy()
    root.background_image = ImageTk.PhotoImage(root.image)
    root.the_sprites = tk.Label(root, image=root.background_image)
    root.the_sprites.pack(fill=tk.BOTH, expand=tk.YES)
    app = MainApplication(root)
    root.the_sprites.bind("<Configure>", app.resize)
    root.mainloop()

else:
    # Command line loop
    image_list = sorted(
        [picture for file_type in [glob.glob(e) for e in [args.input + '/*.jpg', args.input + '/*.png']] for picture in
         file_type])
    sprite_list = [myImageSprite(x=args.cell_width, p=args.padding, file_name=image) for image in image_list]
    gen_sprite_sheet(sprite_list, args.output, args.cell_width, args.n_columns)
