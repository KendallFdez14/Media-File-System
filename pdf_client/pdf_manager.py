import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog, ttk
import requests
import os
import re

API = 'http://localhost:18080'

class PDFManagerApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('Gestor de PDFs RAID 5')
        self.geometry('600x400')
        self.create_widgets()
        self.list_docs()

    def create_widgets(self):
        """Crea los botones y campos de la interfaz gráfica"""
        frame = tk.Frame(self)
        frame.pack(pady=10)
        tk.Button(frame, text='Subir PDF', command=self.upload_pdf).pack(side=tk.LEFT, padx=5)
        tk.Button(frame, text='Descargar', command=self.download_pdf).pack(side=tk.LEFT, padx=5)
        tk.Button(frame, text='Eliminar', command=self.delete_pdf).pack(side=tk.LEFT, padx=5)
        tk.Button(frame, text='Actualizar lista', command=self.list_docs).pack(side=tk.LEFT, padx=5)
        tk.Label(frame, text='Buscar:').pack(side=tk.LEFT, padx=5)
        self.search_var = tk.StringVar()
        tk.Entry(frame, textvariable=self.search_var, width=20).pack(side=tk.LEFT)
        tk.Button(frame, text='Buscar', command=self.search_pdf).pack(side=tk.LEFT, padx=5)

        self.docs_list = tk.Listbox(self, width=80)
        self.docs_list.pack(pady=10, fill=tk.BOTH, expand=True)

    def list_docs(self):
        """Obtiene y muestra la lista de documentos disponibles"""
        self.docs_list.delete(0, tk.END)
        try:
            res = requests.get(f'{API}/status')
            html = res.text
            # Extrae nombres de documentos del HTML
            docs = re.findall(r'Documento: ([^<]+)</h3>', html)
            for doc in docs:
                self.docs_list.insert(tk.END, doc)
        except Exception as e:
            messagebox.showerror('Error', f'No se pudo obtener la lista: {e}')

    def upload_pdf(self):
        """Sube un archivo PDF al sistema distribuido"""
        path = filedialog.askopenfilename(filetypes=[('PDF files', '*.pdf')])
        if not path:
            return
        name = os.path.basename(path)
        try:
            with open(path, 'rb') as f:
                res = requests.post(f'{API}/add?name={name}', data=f.read())
            messagebox.showinfo('Subida', res.text)
            self.list_docs()
        except Exception as e:
            messagebox.showerror('Error', f'No se pudo subir el PDF: {e}')

    def download_pdf(self):
        """Descarga un PDF seleccionado de la lista"""
        sel = self.docs_list.curselection()
        if not sel:
            messagebox.showwarning('Descargar', 'Selecciona un documento de la lista.')
            return
        name = self.docs_list.get(sel[0])
        save_path = filedialog.asksaveasfilename(defaultextension='.pdf', initialfile=name)
        if not save_path:
            return
        try:
            res = requests.get(f'{API}/download?name={name}')
            with open(save_path, 'wb') as f:
                f.write(res.content)
            messagebox.showinfo('Descarga', f'Descargado como {save_path}')
        except Exception as e:
            messagebox.showerror('Error', f'No se pudo descargar: {e}')

    def delete_pdf(self):
        """Elimina un PDF seleccionado de la lista"""
        sel = self.docs_list.curselection()
        if not sel:
            messagebox.showwarning('Eliminar', 'Selecciona un documento de la lista.')
            return
        name = self.docs_list.get(sel[0])
        if not messagebox.askyesno('Eliminar', f'¿Eliminar {name}?'):
            return
        try:
            res = requests.delete(f'{API}/delete?name={name}')
            messagebox.showinfo('Eliminar', res.text)
            self.list_docs()
        except Exception as e:
            messagebox.showerror('Error', f'No se pudo eliminar: {e}')

    def search_pdf(self):
        """Busca un documento por nombre y lo selecciona si existe"""
        name = self.search_var.get().strip()
        if not name:
            return
        try:
            res = requests.get(f'{API}/exists?name={name}')
            if res.json().get('exists'):
                idxs = [i for i in range(self.docs_list.size()) if self.docs_list.get(i) == name]
                if idxs:
                    self.docs_list.selection_clear(0, tk.END)
                    self.docs_list.selection_set(idxs[0])
                    self.docs_list.see(idxs[0])
                else:
                    messagebox.showinfo('Buscar', f'El documento {name} existe pero no está en la lista.')
            else:
                messagebox.showinfo('Buscar', f'No existe el documento {name}.')
        except Exception as e:
            messagebox.showerror('Error', f'No se pudo buscar: {e}')

if __name__ == '__main__':
    # Inicia la aplicación gráfica
    app = PDFManagerApp()
    app.mainloop()
