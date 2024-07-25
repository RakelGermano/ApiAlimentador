from django.urls import path
from . import views

urlpatterns = [
    path('dashboard/', views.dashboard, name='dashboard'),
    path('enviar_mensagem/', views.enviar_mensagem, name='enviar_mensagem'),
    path('get_csrf_token/', views.get_csrf_token, name='get_csrf_token'),
    path('update_button/', views.update_button, name='update_button'),
    path('update_button_view/', views.update_button_view, name='update_button_view'),
    path('', views.index, name='index'),  # Esta linha deve estar correta se a função index existir
]
