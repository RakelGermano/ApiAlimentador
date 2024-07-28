from django.shortcuts import render, redirect
from django.contrib.auth.decorators import login_required
import paho.mqtt.publish as publish
from django.http import JsonResponse
from .models import EstadoBotao
from django.middleware.csrf import get_token
from django.http import HttpResponse
from django.views.decorators.csrf import csrf_exempt
import json

def get_csrf_token(request):
    csrf_token = get_token(request)
    return HttpResponse(csrf_token)

@login_required
def dashboard(request):
    return render(request, 'dashboard.html')

@login_required
def enviar_mensagem(request):
    if request.method == 'POST':
        mensagem = request.POST.get('mensagem', '')
        publish.single(
            topic="Alimentador_TccRecebe",
            payload=mensagem,
            hostname="mqtt.eclipseprojects.io",
            port=1883,
            client_id="Cliente"
        )
    return redirect('dashboard')

@csrf_exempt
def update_button(request):
    if request.method == 'POST':
        try:
            data = json.loads(request.body)
            button = data.get('botao')
            estado = data.get('estado')

            # Atualiza o estado no banco de dados
            estado_botao, created = EstadoBotao.objects.get_or_create(id=1)  # Ajuste conforme sua necessidade
            if button == 1:
                estado_botao.estado1 = (estado == 'botao1Pressionado')
            elif button == 2:
                estado_botao.estado2 = (estado == 'botao2Pressionado')
            estado_botao.save()

            print(f'Botão: {button}, Estado: {estado}')
            return JsonResponse({'status': 'success', 'message': 'Dados recebidos'})
        except json.JSONDecodeError:
            return JsonResponse({'status': 'error', 'message': 'Erro ao processar dados'}, status=400)
    return JsonResponse({'status': 'error', 'message': 'Método não permitido'}, status=405)

def get_button_states(request):
    estado_botao = EstadoBotao.objects.first()
    if estado_botao:
        data = {
            'estado1': 'botao1Pressionado' if estado_botao.estado1 else 'botao1Solto',
            'estado2': 'botao2Pressionado' if estado_botao.estado2 else 'botao2Solto'
        }
    else:
        data = {
            'estado1': 'desconhecido',
            'estado2': 'desconhecido'
        }
    return JsonResponse(data)

def index(request):
    try:
        estado_botao = EstadoBotao.objects.get(id=1)
    except EstadoBotao.DoesNotExist:
        estado_botao = None
    return render(request, 'dashboard.html', {'estado': estado_botao.estado if estado_botao else False})
