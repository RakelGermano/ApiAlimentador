# Generated by Django 3.2.25 on 2024-07-28 17:17

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Dashboard', '0002_auto_20240728_1407'),
    ]

    operations = [
        migrations.AddField(
            model_name='estadobotao',
            name='updated_at',
            field=models.DateTimeField(auto_now=True),
        ),
        migrations.AlterField(
            model_name='estadobotao',
            name='estado1',
            field=models.CharField(max_length=10),
        ),
        migrations.AlterField(
            model_name='estadobotao',
            name='estado2',
            field=models.CharField(max_length=10),
        ),
    ]
