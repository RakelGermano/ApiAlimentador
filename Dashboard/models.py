from django.db import models

class EstadoBotao(models.Model):
    estado1 = models.BooleanField(default=False)
    estado2 = models.BooleanField(default=False)
    updated_at = models.DateTimeField(auto_now=True)

    def __str__(self):
        return f'Botão 1: {"Pressionado" if self.estado1 else "Solto"}, Botão 2: {"Pressionado" if self.estado2 else "Solto"}'
