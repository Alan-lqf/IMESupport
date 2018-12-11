import sublime
import sublime_plugin

try:
    from imesupport import globalhook
except ImportError:
    from .imesupport import globalhook


class SlackCommand(sublime_plugin.TextCommand):
    def run(self, edit):
        self.view.insert(edit, 0, "Hello, World!")


class WindowLayout(object):
    def __init__(self, window):
        self.window = window
        self.last_extents = None
        self.settings = sublime.load_settings('IMESupport.sublime-settings')

    def calc_cursor_position(self, view, cursor):
        if "text_to_window" in dir(view):
            pos = view.text_to_window(cursor)
            font_size = self.get_font_size(view)
            caret_width = view.settings().get('caret_extra_width')
            scaling = self.settings.get('imesupport_screen_scaling')
            return (int(pos[0]) + caret_width, int(pos[1]), font_size, scaling)

    @staticmethod
    def get_font_face(view):
        return view.settings().get('font_face', '')

    @staticmethod
    def get_font_size(view):
        return int(view.settings().get('font_size', ''))


def setup(hwnd):
    if int(sublime.version()) < 3000:
        pass
    else:

        # load dll
        globalhook.setup(hwnd, sublime.arch() == 'x64')


class ImeSupportEventListener(sublime_plugin.EventListener):
    def __init__(self):
        self.layouts = {}
        self.initialized = False

    def on_activated(self, view):
        self.update(view)

    def on_modified(self, view):
        self.update(view)

    def on_selection_modified(self, view):
        self.update(view)

    def on_post_window_command(self, window, command_name, args):
        sublime.set_timeout_async(self.update(window.active_view()), 400)

    def update(self, view):
        if view is None:
            return
        window = view.window()
        if window is None:
            return

        if not self.initialized:
            setup(window.hwnd())
            with open('font_name.txt', 'wb') as f:
                f.write(view.settings().get('font_face', '').encode('utf-8'))

            self.initialized = True

        id = window.id()
        if id not in self.layouts:
            self.layouts[id] = WindowLayout(window)

        pos = self.layouts[id].calc_cursor_position(view, view.sel()[0].b)
        globalhook.set_inline_position(window.hwnd(), *pos)


def plugin_unload():
    globalhook.cleanup()
