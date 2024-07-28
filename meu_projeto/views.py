# minha_aplicacao/views.py
from django.shortcuts import render, redirect
from django.shortcuts import render
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_protect

def get_csrf_token(request):
    csrf_token = get_token(request)
    return HttpResponse(csrf_token)

def CriarConta_view(request):
    if request.method == 'POST':
        form = UserCreationForm(request.POST)
        if form.is_valid():
            form.save()
            # Redirecionar para a página de login após o CriarConta bem-sucedido
            return redirect('login')
    else:
        form = UserCreationForm()
    return render(request, 'CriarConta.html', {'form': form})

def login_redirect(request):
    return redirect('login')

def TelaInicial(request):
    return render(request, 'TelaInicial.html')

def login_view(request):
    if request.method == 'POST':
        username = request.POST['username']
        password = request.POST['password']
        user = authenticate(request, username=username, password=password)
        if user is not None:
            login(request, user)
            return redirect('dashboard')
        else:
            return render(request, 'login.html', {'error': 'Credenciais inválidas'})
    return render(request, 'login.html')

# @csrf_protect
# def update_button(request):
#     if request.method == 'POST':
#         try:
#             data = json.loads(request.body)
#             button = data.get('botao')
#             estado = data.get('estado')

#             # Aqui você pode adicionar lógica para atualizar o estado dos botões
#             # e animar os círculos na página do dashboard.
#             # Exemplo:
#             #print(f'Botão: {button}, Estado: {estado}')

#             # Supondo que você atualize alguma variável global ou banco de dados
#             # que a página do dashboard lê para saber como animar os círculos.

#             return JsonResponse({'status': 'success', 'message': 'Dados recebidos'})
#         except json.JSONDecodeError:
#             return JsonResponse({'status': 'error', 'message': 'Erro ao processar dados'}, status=400)
#     return JsonResponse({'status': 'error', 'message': 'Método não permitido'}, status=405)

def logout_view(request):
    logout(request)
    return redirect('login')
