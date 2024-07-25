from django.urls import path, include
from meu_projeto import views as meu_projeto_views
from Dashboard import views as dashboard_views

urlpatterns = [
    path('', meu_projeto_views.TelaInicial, name='TelaInicial'),
    path('login/', meu_projeto_views.login_view, name='login'),
    path('logout/', meu_projeto_views.logout_view, name='logout'),
    path('CriarConta/', meu_projeto_views.CriarConta_view, name='CriarConta'),
    path('Dashboard', dashboard_views.dashboard, name='dashboard'),
    path('enviar_mensagem', dashboard_views.enviar_mensagem, name='enviar_mensagem'),
    # path('update_button/', dashboard_views.update_button, name='update_button'),
    path('', include('Dashboard.urls')),
]
