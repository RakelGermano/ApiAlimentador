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

@csrf_protect
def update_button(request):
    if request.method == 'POST':
        estado = request.POST.get('estado')
        if estado is not None:
            estado_botao, created = EstadoBotao.objects.get_or_create(id=1)
            estado_botao.estado = bool(int(estado))
            estado_botao.save()
            # Aqui você pode adicionar lógica adicional, se necessário
            return redirect('dashboard')  # Redireciona para a view ou URL com nome 'dashboard'
        else:
            return JsonResponse({'status': 'erro'})
    else:
        return JsonResponse({'status': 'erro'})

def logout_view(request):
    logout(request)
    return redirect('login')
