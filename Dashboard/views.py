from django.shortcuts import render, redirect
from django.contrib.auth.decorators import login_required
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt_client
from django.http import JsonResponse, HttpResponse
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
@login_required
def update_button_view(request):
    if request.method == 'GET':
        try:
            estado_botao = EstadoBotao.objects.get(id=1)
            estado = '1' if estado_botao.estado else '0'
            return JsonResponse({'estado': estado})
        except EstadoBotao.DoesNotExist:
            return JsonResponse({'estado': '0'})
    return JsonResponse({'error': 'Invalid request method'}, status=405)


@csrf_exempt
def update_button(request):
    if request.method == 'POST':
        try:
            data = json.loads(request.body.decode('utf-8'))
            estado = data.get('estado')

            # Salvar o estado no banco de dados (exemplo com EstadoBotao)
            estado_obj, created = EstadoBotao.objects.get_or_create(id=1)
            estado_obj.estado = int(estado)  # Converte para inteiro, se necessário
            estado_obj.save()

            print(f"Estado do botão salvo no banco de dados: {estado}")
            return JsonResponse({'status': 'sucesso', 'estado': estado})
        except json.JSONDecodeError:
            return JsonResponse({'status': 'erro', 'mensagem': 'JSON inválido'}, status=400)
    else:
        return JsonResponse({'status': 'erro', 'mensagem': 'Método não permitido'}, status=405)
    
def index(request):
    try:
        estado_botao = EstadoBotao.objects.get(id=1)
    except EstadoBotao.DoesNotExist:
        estado_botao = None
    return render(request, 'dashboard.html', {'estado': estado_botao.estado if estado_botao else False})
