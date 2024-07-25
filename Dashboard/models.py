# models.py
from django.db import models

class EstadoBotao(models.Model):
    estado = models.BooleanField(default=False)

    def __str__(self):
        return f"Estado: {'Apertado' if self.estado else 'Solto'}"
